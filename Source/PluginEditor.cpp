#include "PluginEditor.h"

AutumnVeilAudioProcessorEditor::AutumnVeilAudioProcessorEditor(AutumnVeilAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    auto& apvts = audioProcessor.getAPVTS();

    auto setupSlider = [&](juce::Slider& s, juce::Label& l, const juce::String& id, const juce::String& name) {
        addAndMakeVisible(s);
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(l);
        l.setText(name, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.attachToComponent(&s, false);
    };

    setupSlider(cutoffSlider, cutoffLabel, "cutoff", "Cutoff");
    setupSlider(resSlider, resLabel, "resonance", "Resonance");
    setupSlider(attackSlider, attackLabel, "attack", "Attack");
    setupSlider(decaySlider, decayLabel, "decay", "Decay");
    setupSlider(sustainSlider, sustainLabel, "sustain", "Sustain");
    setupSlider(releaseSlider, releaseLabel, "release", "Release");
    setupSlider(atSlider, atLabel, "atAmount", "AT Amount");

    cutoffAtt = std::make_unique<Attachment>(apvts, "cutoff", cutoffSlider);
    resAtt = std::make_unique<Attachment>(apvts, "resonance", resSlider);
    attackAtt = std::make_unique<Attachment>(apvts, "attack", attackSlider);
    decayAtt = std::make_unique<Attachment>(apvts, "decay", decaySlider);
    sustainAtt = std::make_unique<Attachment>(apvts, "sustain", sustainSlider);
    releaseAtt = std::make_unique<Attachment>(apvts, "release", releaseSlider);
    atAtt = std::make_unique<Attachment>(apvts, "atAmount", atSlider);

    setSize(700, 420);
}

AutumnVeilAudioProcessorEditor::~AutumnVeilAudioProcessorEditor() = default;

void AutumnVeilAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0a0a12));
    g.setColour(juce::Colour(0xff1a1a2e));
    g.fillRoundedRectangle(getLocalBounds().reduced(8).toFloat(), 12.0f);

    g.setColour(juce::Colour(0xffc9a227));
    g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    g.drawText("Autumn Veil", 20, 16, 300, 36, juce::Justification::left);

    g.setColour(juce::Colour(0xff8899aa));
    g.setFont(juce::FontOptions(14.0f));
    g.drawText("Aftertouch breathes the veil  •  Sustain pedal holds the mist", 20, 52, 500, 20, juce::Justification::left);

    g.setColour(juce::Colour(0x33c9a227));
    g.drawRoundedRectangle(getLocalBounds().reduced(6).toFloat(), 14.0f, 1.5f);
}

void AutumnVeilAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20).removeFromBottom(320);
    int w = area.getWidth() / 7;
    cutoffSlider.setBounds(area.removeFromLeft(w).reduced(8));
    resSlider.setBounds(area.removeFromLeft(w).reduced(8));
    attackSlider.setBounds(area.removeFromLeft(w).reduced(8));
    decaySlider.setBounds(area.removeFromLeft(w).reduced(8));
    sustainSlider.setBounds(area.removeFromLeft(w).reduced(8));
    releaseSlider.setBounds(area.removeFromLeft(w).reduced(8));
    atSlider.setBounds(area.reduced(8));
}
