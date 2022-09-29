/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
True_stereo_pannerAudioProcessor::True_stereo_pannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()), panner()
#endif
{
    // Initilisation of the audio parameters pointers
    castParameter(apvts, ParameterID::panValue, pan_param);
    castParameter(apvts, ParameterID::panMethod, pan_method_param);
    castParameter(apvts, ParameterID::widthValue, width_param);
    castParameter(apvts, ParameterID::monoPannerRule, mono_rule_param);
    castParameter(apvts, ParameterID::stereoPannerRule, stereo_rule_param);
    castParameter(apvts, ParameterID::lfoActive, lfo_active_param);
    castParameter(apvts, ParameterID::lfoAmount, lfo_amount_param);
    castParameter(apvts, ParameterID::lfoSynced, lfo_synced_param);
    castParameter(apvts, ParameterID::lfoRateHz, lfo_rate_hz_param);
    castParameter(apvts, ParameterID::lfoRateSync, lfo_rate_sync_param);
    castParameter(apvts, ParameterID::lfoWaveform, lfo_waveform_param);
    
    apvts.addParameterListener(ParameterID::panValue.getParamID(), this);
    apvts.addParameterListener(ParameterID::widthValue.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoSynced.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoWaveform.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoRateHz.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoRateSync.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoAmount.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoActive.getParamID(), this);
    
    current_position_info = new juce::AudioPlayHead::CurrentPositionInfo();
}

True_stereo_pannerAudioProcessor::~True_stereo_pannerAudioProcessor()
{
    apvts.removeParameterListener(ParameterID::panValue.getParamID(), this);
    apvts.removeParameterListener(ParameterID::widthValue.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoSynced.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoWaveform.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoRateHz.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoRateSync.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoAmount.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoActive.getParamID(), this);
}

//==============================================================================
const juce::String True_stereo_pannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool True_stereo_pannerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool True_stereo_pannerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool True_stereo_pannerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double True_stereo_pannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int True_stereo_pannerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int True_stereo_pannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void True_stereo_pannerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String True_stereo_pannerAudioProcessor::getProgramName (int index)
{
    return {};
}

void True_stereo_pannerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    
}

//==============================================================================
void True_stereo_pannerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    panner.prepare(spec);
    getPlayHead()->getCurrentPosition(*current_position_info);
    last_bpm = current_position_info->bpm;
}

void True_stereo_pannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool True_stereo_pannerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void True_stereo_pannerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto audioBlock = juce::dsp::AudioBlock<float> (buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float> (audioBlock);
    
    panner.set_pan_method((panMethod) pan_method_param->getIndex());
    
    if (panner.get_lfo_active()) {
        if (panner.get_lfo_synced()) {
            getPlayHead()->getCurrentPosition(*current_position_info);
            float bpm = current_position_info->bpm;
            if ((bpm - last_bpm) > __FLT_EPSILON__) { // bpm != last_bpm
                const juce::String& paramID = ParameterID::lfoRateSync.getParamID();
                panner.set_lfo_rate_synced((sync_rate_t) apvts.getRawParameterValue(paramID)->load(), bpm);
                last_bpm = bpm;
            }
        } else {
            const juce::String& paramID = ParameterID::lfoRateHz.getParamID();
            if ((apvts.getRawParameterValue(paramID)->load() - last_rate_hz) > __FLT_EPSILON__) {
                panner.set_lfo_rate_hz(apvts.getRawParameterValue(paramID)->load());
            }
        }
    }
    
    switch (panner.get_pan_method()) {
        case MONO_PANNER:
            panner.set_mono_panner_rule((juce::dsp::PannerRule) mono_rule_param->getIndex());
            break;
        case STEREO_PANNER:
            panner.set_stereo_panner_rule((stereoPannerRule) stereo_rule_param->getIndex());
            break;
        default:
            break;
    }
    panner.process(processContext);
    
    // Limiting to avoid any unexpected ear loss or speaker damage..
    limitOutput(buffer.getWritePointer(0), buffer.getNumSamples());
    limitOutput(buffer.getWritePointer(1), buffer.getNumSamples());
}

//==============================================================================
bool True_stereo_pannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* True_stereo_pannerAudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 750);
    return editor;
}

//==============================================================================
void True_stereo_pannerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void True_stereo_pannerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new True_stereo_pannerAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout True_stereo_pannerAudioProcessor::createParameters() {
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::AudioProcessorValueTreeState::ParameterLayout paramsLayout;
    const juce::StringArray rate_sync_names = {"1/64", "1/48", "1/32", "1/16", "1/12", "1/8", "1/6", "1/4", "1/3", "1/2", "1", "2", "3", "4", "6", "8", "16"};
    
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::panMethod, "Panning Method", juce::StringArray {"Mono", "Stereo", "Binaural"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::monoPannerRule, "Mono Panner Rule", juce::StringArray {"Linear", "Balanced", "Sin3dB", "sin4p5dB", "sin6dB", "squareroot3db", "squareroot4p5db"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::stereoPannerRule, "Stereo Panner Rule", juce::StringArray{"Linear", "Sin3dB"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::panValue, "Panning", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::widthValue, "Width (only in stereo panner)", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 0.3f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool> (ParameterID::lfoActive, "Active autopan LFO", 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::lfoWaveform, "Waveform LFO", juce::StringArray {"Sine", "Saw", "Square", "Triangle"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::lfoAmount, "Amount LFO", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID::lfoSynced, "LFO Synced", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::lfoRateHz, "Lfo Rate (Hz)", juce::NormalisableRange<float>(0.01, 10.0f, 0.01, 0.3), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::lfoRateSync, "Lfo Rate (Sync)", rate_sync_names, 11));
    
    paramsLayout.add(params.begin(), params.end());
    
    return paramsLayout;
}

void True_stereo_pannerAudioProcessor::parameterChanged(const juce::String& paramID, float newValue) {
    if (paramID == ParameterID::panValue.getParamID()) {
        panner.set_pan(static_cast<float>(newValue));
        return;
    }
    
    if (paramID == ParameterID::widthValue.getParamID()) {
        panner.set_stereo_width(static_cast<float>(newValue));
        return;
    }
    
    if (paramID == ParameterID::lfoActive.getParamID()) {
        panner.set_lfo_active(static_cast<bool>(newValue));
        return;
    }
    
    if (paramID == ParameterID::lfoSynced.getParamID()) {
        panner.set_lfo_synced(static_cast<bool>(newValue), last_bpm);
        return;
    }
    
//    if (paramID == ParameterID::lfoRateHz.getParamID()) {
//        // freq of the LFO is multiplied by fs/block_size because we increment
//        // the lfo output value only at every processBlock() call, which is enough
//        // for a lfo with a max freq rate of 10Hz. Might reconsider design and add an lfo on each type of panner and update lfo output at every sample (which can be useful for higher lfo frequency rate
//        if (!panner.lfo_sync) {
//            float freq = static_cast<float>(newValue) * (getSampleRate() / getBlockSize());
//            panner.set_lfo_rate_hz(freq);
//        }
//        return;
//    }
//
//    if (paramID == ParameterID::lfoRateSync.getParamID()) {
//        if (panner.lfo_sync) {
//            float rate_in_hz = get_rate_in_hz(static_cast<sync_rate_t>(newValue));
//            float freq = rate_in_hz * (getSampleRate() / getBlockSize() );
//            panner.set_lfo_rate_synced(freq);
//        }
//        return;
//    }
    
    if (paramID == ParameterID::lfoAmount.getParamID()) {
        panner.set_lfo_amount(newValue);
        return;
    }
    
    if (paramID == ParameterID::lfoWaveform.getParamID()) {
        panner.set_lfo_waveform(static_cast<waveform_t>(newValue));
        return;
    }
}

