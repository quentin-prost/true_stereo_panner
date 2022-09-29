/*
  ==============================================================================

    Oscillator.h
    Created: 29 Sep 2022 6:25:23pm
    Author:  Quentin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/* Taken from the Oscillator class from the JUCE framework but needed to slightly change the class to be able to
 change the number of interpolation set when changing the frequency of the oscilattor, as the <SmoothedValue>Frequency is private and cannot be accessed from*/
template <typename SampleType>
class Oscillator
{
public:
    /** The NumericType is the underlying primitive type used by the SampleType (which
     could be either a primitive or vector)
     */
    using NumericType = typename juce::dsp::SampleTypeHelpers::ElementType<SampleType>::Type;
    
    /** Creates an uninitialised oscillator. Call initialise before first use. */
    Oscillator() = default;
    
    /** Creates an oscillator with a periodic input function (-pi..pi).
     
     If lookup table is not zero, then the function will be approximated
     with a lookup table.
     */
    Oscillator (const std::function<NumericType (NumericType)>& function,
                size_t lookupTableNumPoints = 0)
    {
    initialise (function, lookupTableNumPoints);
    }
    
    /** Returns true if the Oscillator has been initialised. */
    bool isInitialised() const noexcept     { return static_cast<bool> (generator); }
    
    /** Initialises the oscillator with a waveform. */
    void initialise (const std::function<NumericType (NumericType)>& function,
                     size_t lookupTableNumPoints = 0)
    {
    if (lookupTableNumPoints != 0)
        {
        auto* table = new juce::dsp::LookupTableTransform<NumericType> (function,
                                                                        -juce::MathConstants<NumericType>::pi,
                                                                        juce::MathConstants<NumericType>::pi,
                                                             lookupTableNumPoints);
        
        lookupTable.reset (table);
        generator = [table] (NumericType x) { return (*table) (x); };
        }
    else
        {
        generator = function;
        }
    }
    
    //==============================================================================
    /** Sets the frequency of the oscillator. */
    void setFrequency (NumericType newFrequency, bool force = false) noexcept
    {
    if (force)
        {
        frequency.setCurrentAndTargetValue (newFrequency);
        return;
        }
    
    frequency.setTargetValue (newFrequency);
    DBG(frequency.getCurrentValue());
    }
    
    /** Returns the current frequency of the oscillator. */
    NumericType getFrequency() const noexcept                    { return frequency.getTargetValue(); }
    
    //==============================================================================
    /** Called before processing starts. */
    void prepare (const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = static_cast<NumericType> (spec.sampleRate);
        rampBuffer.resize ((int) spec.maximumBlockSize);
        
        reset();
    }
    
    /** Resets the internal state of the oscillator */
    void reset() noexcept
    {
        phase.reset();
        
        if (sampleRate > 0)
            frequency.reset (sampleRate, 0.001);
    }
    
    //==============================================================================
    /** Returns the result of processing a single sample. */
    SampleType JUCE_VECTOR_CALLTYPE processSample (SampleType input) noexcept
    {
        jassert (isInitialised());
        auto increment = juce::MathConstants<NumericType>::twoPi * frequency.getNextValue() / sampleRate;
        return input + generator (phase.advance (increment) - juce::MathConstants<NumericType>::pi);
    }
    
    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        jassert (isInitialised());
        auto&& outBlock = context.getOutputBlock();
        auto&& inBlock  = context.getInputBlock();
        
        // this is an output-only processor
        jassert (outBlock.getNumSamples() <= static_cast<size_t> (rampBuffer.size()));
        
        auto len           = outBlock.getNumSamples();
        auto numChannels   = outBlock.getNumChannels();
        auto inputChannels = inBlock.getNumChannels();
        auto baseIncrement = juce::MathConstants<NumericType>::twoPi / sampleRate;
        
    if (context.isBypassed)
        context.getOutputBlock().clear();
    
    if (frequency.isSmoothing())
        {
        auto* buffer = rampBuffer.getRawDataPointer();
        
        for (size_t i = 0; i < len; ++i)
            buffer[i] = phase.advance (baseIncrement * frequency.getNextValue())
            - juce::MathConstants<NumericType>::pi;
        
        if (! context.isBypassed)
            {
            size_t ch;
            
            if (context.usesSeparateInputAndOutputBlocks())
                {
                for (ch = 0; ch < jmin (numChannels, inputChannels); ++ch)
                    {
                    auto* dst = outBlock.getChannelPointer (ch);
                    auto* src = inBlock.getChannelPointer (ch);
                    
                    for (size_t i = 0; i < len; ++i)
                        dst[i] = src[i] + generator (buffer[i]);
                    }
                }
            else
                {
                for (ch = 0; ch < jmin (numChannels, inputChannels); ++ch)
                    {
                    auto* dst = outBlock.getChannelPointer (ch);
                    
                    for (size_t i = 0; i < len; ++i)
                        dst[i] += generator (buffer[i]);
                    }
                }
            
            for (; ch < numChannels; ++ch)
                {
                auto* dst = outBlock.getChannelPointer (ch);
                
                for (size_t i = 0; i < len; ++i)
                    dst[i] = generator (buffer[i]);
                }
            }
        }
    else
        {
        auto freq = baseIncrement * frequency.getNextValue();
        auto p = phase;
        
        if (context.isBypassed)
            {
                frequency.skip (static_cast<int> (len));
                p.advance (freq * static_cast<NumericType> (len));
            }
        else
            {
            size_t ch;
            
            if (context.usesSeparateInputAndOutputBlocks())
                {
                for (ch = 0; ch < jmin (numChannels, inputChannels); ++ch)
                    {
                    p = phase;
                    auto* dst = outBlock.getChannelPointer (ch);
                    auto* src = inBlock.getChannelPointer (ch);
                    
                    for (size_t i = 0; i < len; ++i)
                        dst[i] = src[i] + generator (p.advance (freq) - juce::MathConstants<NumericType>::pi);
                    }
                }
            else
                {
                for (ch = 0; ch < jmin (numChannels, inputChannels); ++ch)
                    {
                    p = phase;
                    auto* dst = outBlock.getChannelPointer (ch);
                    
                    for (size_t i = 0; i < len; ++i)
                        dst[i] += generator (p.advance (freq) - juce::MathConstants<NumericType>::pi);
                    }
                }
            
            for (; ch < numChannels; ++ch)
                {
                p = phase;
                auto* dst = outBlock.getChannelPointer (ch);
                
                for (size_t i = 0; i < len; ++i)
                    dst[i] = generator (p.advance (freq) - juce::MathConstants<NumericType>::pi);
                }
            }
        
        phase = p;
        }
    }
    
protected:
    juce::SmoothedValue<NumericType> frequency { static_cast<NumericType> (440.0) };
    juce::dsp::Phase<NumericType> phase;
    NumericType sampleRate = 48000.0;
    
private:
    //==============================================================================
    std::function<NumericType (NumericType)> generator;
    std::unique_ptr<juce::dsp::LookupTableTransform<NumericType>> lookupTable;
    juce::Array<NumericType> rampBuffer;
    
    
};
