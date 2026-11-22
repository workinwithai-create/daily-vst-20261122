#pragma once
#include <JuceHeader.h>

class VeilSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class VeilVoice : public juce::SynthesiserVoice
{
public:
    VeilVoice();
    bool canPlaySound(juce::SynthesiserSound*) override { return true; }
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void aftertouchChanged(int newAftertouchValue) override;
    void channelPressureChanged(int newChannelPressureValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setADSR(const juce::ADSR::Parameters& params);
    void setFilter(float cutoffHz, float resonance);
    void setAftertouchAmount(float amount);

private:
    double currentAngle = 0.0, angleDelta = 0.0;
    float level = 0.0f;
    float aftertouch = 0.0f;
    float aftertouchAmount = 0.5f;
    juce::ADSR adsr;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::ProcessSpec spec;
};

class AutumnVeilAudioProcessor : public juce::AudioProcessor
{
public:
    AutumnVeilAudioProcessor();
    ~AutumnVeilAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;
    juce::dsp::Limiter<float> limiter;
    juce::SmoothedValue<float> cutoffSmoothed, resonanceSmoothed, aftertouchAmountSmoothed;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutumnVeilAudioProcessor)
};
