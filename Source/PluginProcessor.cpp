#include "PluginProcessor.h"
#include "PluginEditor.h"

VeilVoice::VeilVoice()
{
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

void VeilVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentAngle = 0.0;
    level = velocity * 0.15f;
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    angleDelta = cyclesPerSecond / getSampleRate() * juce::MathConstants<double>::twoPi;
    adsr.noteOn();
}

void VeilVoice::stopNote(float, bool allowTailOff)
{
    adsr.noteOff();
    if (!allowTailOff)
        clearCurrentNote();
}

void VeilVoice::aftertouchChanged(int newAftertouchValue)
{
    aftertouch = newAftertouchValue / 127.0f;
}

void VeilVoice::channelPressureChanged(int newChannelPressureValue)
{
    aftertouch = newChannelPressureValue / 127.0f;
}

void VeilVoice::setADSR(const juce::ADSR::Parameters& params) { adsr.setParameters(params); }

void VeilVoice::setFilter(float cutoffHz, float resonance)
{
    filter.setCutoffFrequency(cutoffHz);
    filter.setResonance(resonance);
}

void VeilVoice::setAftertouchAmount(float amount) { aftertouchAmount = amount; }

void VeilVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta == 0.0) return;

    auto* left = outputBuffer.getWritePointer(0, startSample);
    auto* right = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        auto env = adsr.getNextSample();
        if (env <= 0.0f && !adsr.isActive())
        {
            clearCurrentNote();
            angleDelta = 0.0;
            break;
        }

        auto sample = static_cast<float>(std::sin(currentAngle)) * level * env;
        sample *= (0.6f + 0.4f * aftertouch * aftertouchAmount);

        float filtered = filter.processSample(0, sample);
        if (right) filter.processSample(1, sample);

        left[i] += filtered;
        if (right) right[i] += filtered;

        currentAngle += angleDelta;
        if (currentAngle >= juce::MathConstants<double>::twoPi)
            currentAngle -= juce::MathConstants<double>::twoPi;
    }
}

//==============================================================================
AutumnVeilAudioProcessor::AutumnVeilAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new VeilVoice());
    synth.addSound(new VeilSound());
}

AutumnVeilAudioProcessor::~AutumnVeilAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout AutumnVeilAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", 80.0f, 12000.0f, 800.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance", 0.1f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.01f, 5.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.01f, 5.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.01f, 8.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("atAmount", "Aftertouch Amount", 0.0f, 1.0f, 0.6f));
    return { params.begin(), params.end() };
}

void AutumnVeilAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
    limiter.prepare(spec);
    limiter.setThreshold(-1.0f);
    limiter.setRelease(100.0f);

    cutoffSmoothed.reset(sampleRate, 0.05);
    resonanceSmoothed.reset(sampleRate, 0.05);
    aftertouchAmountSmoothed.reset(sampleRate, 0.05);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<VeilVoice*>(synth.getVoice(i)))
            v->filter.prepare(spec);
}

void AutumnVeilAudioProcessor::releaseResources() {}

bool AutumnVeilAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AutumnVeilAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    auto cutoff = apvts.getRawParameterValue("cutoff")->load();
    auto res = apvts.getRawParameterValue("resonance")->load();
    auto atAmt = apvts.getRawParameterValue("atAmount")->load();
    cutoffSmoothed.setTargetValue(cutoff);
    resonanceSmoothed.setTargetValue(res);
    aftertouchAmountSmoothed.setTargetValue(atAmt);

    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = apvts.getRawParameterValue("attack")->load();
    adsrParams.decay = apvts.getRawParameterValue("decay")->load();
    adsrParams.sustain = apvts.getRawParameterValue("sustain")->load();
    adsrParams.release = apvts.getRawParameterValue("release")->load();

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<VeilVoice*>(synth.getVoice(i)))
        {
            v->setADSR(adsrParams);
            v->setFilter(cutoffSmoothed.getNextValue(), resonanceSmoothed.getNextValue());
            v->setAftertouchAmount(aftertouchAmountSmoothed.getNextValue());
        }
    }

    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    limiter.process(ctx);
}

juce::AudioProcessorEditor* AutumnVeilAudioProcessor::createEditor()
{
    return new AutumnVeilAudioProcessorEditor(*this);
}

void AutumnVeilAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AutumnVeilAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutumnVeilAudioProcessor();
}
