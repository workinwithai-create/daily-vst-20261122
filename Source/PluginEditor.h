#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AutumnVeilAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AutumnVeilAudioProcessorEditor(AutumnVeilAudioProcessor&);
    ~AutumnVeilAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AutumnVeilAudioProcessor& audioProcessor;

    juce::Slider cutoffSlider, resSlider, attackSlider, decaySlider, sustainSlider, releaseSlider, atSlider;
    juce::Label cutoffLabel, resLabel, attackLabel, decayLabel, sustainLabel, releaseLabel, atLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> cutoffAtt, resAtt, attackAtt, decayAtt, sustainAtt, releaseAtt, atAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutumnVeilAudioProcessorEditor)
};
