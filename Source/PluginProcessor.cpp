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
    
    apvts.addParameterListener(ParameterID::panValue.getParamID(), this);
    apvts.addParameterListener(ParameterID::widthValue.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoSynced.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoRateHz.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoRateSync.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoAmount.getParamID(), this);
    apvts.addParameterListener(ParameterID::lfoActive.getParamID(), this);
}

True_stereo_pannerAudioProcessor::~True_stereo_pannerAudioProcessor()
{
    apvts.removeParameterListener(ParameterID::panValue.getParamID(), this);
    apvts.removeParameterListener(ParameterID::widthValue.getParamID(), this);
    apvts.removeParameterListener(ParameterID::lfoSynced.getParamID(), this);
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
    
    play_head = getPlayHead();
    play_head->getCurrentPosition(current_position_info);
    
    panner.set_pan_method((panMethod) pan_method_param->getIndex());
    
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

//void True_stereo_pannerAudioProcessor::set_pan(float pan) {
//    m_pan = pan;
//    panner.set_pan(pan);
//}

//void True_stereo_pannerAudioProcessor::set_pan_method(panMethod method) {
//    panner.set_pan_method(method);
//    m_method = method;
//}

juce::AudioProcessorValueTreeState::ParameterLayout True_stereo_pannerAudioProcessor::createParameters() {
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::AudioProcessorValueTreeState::ParameterLayout paramsLayout;
    juce::StringArray rate_sync_names = {"1/64", "1/48", "1/32", "1/16", "1/8", "1/6", "1/4", "1/3", "1/2", "1", "2", "3", "4", "8", "16"};
    
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::panMethod, "Panning Method", juce::StringArray {"Mono", "Stereo", "Binaural"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::monoPannerRule, "Mono Panner Rule", juce::StringArray {"Linear", "Balanced", "Sin3dB", "sin4p5dB", "sin6dB", "squareroot3db", "squareroot4p5db"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::stereoPannerRule, "Stereo Panner Rule", juce::StringArray{"Linear", "Sin3dB"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::panValue, "Panning", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::widthValue, "Width", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 0.3f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool> (ParameterID::lfoActive, "Active LFO", 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::lfoAmount, "Amount LFO", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID::lfoSynced, "LFO Synced", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (ParameterID::lfoRateHz, "Lfo rate (Hz)", juce::NormalisableRange<float>(0.01, 10.0f, 0.02, 0.3), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice> (ParameterID::lfoRateSync, "Lfo Rate Sync", rate_sync_names, 9));
    
    paramsLayout.add(params.begin(), params.end());
    
    return paramsLayout;
}

void True_stereo_pannerAudioProcessor::parameterChanged(const juce::String& paramID, float newValue) {
    if (paramID == ParameterID::panValue.getParamID()) {
        panner.set_pan(newValue);
        return;
    }
    
    if (paramID == ParameterID::widthValue.getParamID()) {
        panner.set_width(newValue);
        return;
    }
    
    if (paramID == ParameterID::lfoActive.getParamID()) {
        panner.set_lfo_active(newValue);
        return;
    }
    
    if (paramID == ParameterID::lfoRateHz.getParamID()) {
        panner.set_lfo_rate_hz(newValue);
        return;
    }
    
    if (paramID == ParameterID::lfoRateSync.getParamID()) {
        float rate_in_hz = get_rate_in_hz(static_cast<sync_rate_t>(newValue));
        panner.set_lfo_rate_hz(rate_in_hz);
        return;
    }
    
    if (paramID == ParameterID::lfoAmount.getParamID()) {
        panner.set_lfo_amount(newValue);
        return;
    }
}

float True_stereo_pannerAudioProcessor::get_rate_in_hz(sync_rate_t rate) {
    float rate_hz;
    float bpm = current_position_info.bpm;
    switch (rate) {
        case T1_16: rate_hz = 16.0f * bpm / 60.0f;
            break;
        case T1_12: rate_hz = 12.0f * bpm / 60.0f;
            break;
        case T1_8: rate_hz = 8.0f * bpm / 60.0f;
            break;
        case T1_6: rate_hz = 6.0f * bpm / 60.0f;
            break;
        case T1_4: rate_hz = 4.0f * bpm / 60.0f;
            break;
        case T1_3: rate_hz = 3.0f * bpm / 60.0f;
            break;
        case T1_2: rate_hz = 2.0f * bpm / 60.0f;
            break;
        case T1_1: rate_hz = 1.0f * bpm / 60.0f;
            break;
        case T2_1: rate_hz = 0.5f * bpm / 60.0f;
            break;
        case T3_1: rate_hz = (1/3.0f) * bpm / 60.0f;
            break;
        case T4_1: rate_hz = (1/2.0f) * bpm / 60.0f;
            break;
        case T8_1: rate_hz = (1/8.0f) * bpm / 60.0f;
            break;
        case T16_1: rate_hz = (1/16.0f) * bpm / 60.0f;
            break;
        default: rate_hz = bpm / 60.0f;
            break;
    }
    return rate_hz;
    
}
