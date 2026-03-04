#include "PluginProcessor.h"
#include "PluginEditor.h"

SkuxAudioProcessorEditor::SkuxAudioProcessorEditor(SkuxAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), oscilloscope(p.getScopeQueue())
{
  setLookAndFeel(&skuxLookAndFeel);

  addAndMakeVisible(oscilloscope);

  addAndMakeVisible(driveKnob);
  addAndMakeVisible(mixKnob);
  addAndMakeVisible(distTypeBox);
  addAndMakeVisible(cutoffKnob);
  addAndMakeVisible(qKnob);
  addAndMakeVisible(filterRoutingBox);

  distortionSectionLabel.setText("DISTORTION", juce::dontSendNotification);
  distortionSectionLabel.setJustificationType(juce::Justification::centred);
  distortionSectionLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
  distortionSectionLabel.setFont(juce::FontOptions(13.f, juce::Font::bold));
  addAndMakeVisible(distortionSectionLabel);

  filterSectionLabel.setText("FILTER", juce::dontSendNotification);
  filterSectionLabel.setJustificationType(juce::Justification::centred);
  filterSectionLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
  filterSectionLabel.setFont(juce::FontOptions(13.f, juce::Font::bold));
  addAndMakeVisible(filterSectionLabel);
  
  distTypeBox.comboBox.addItemList({"Soft Clip", "Hard Clip"}, 1);
  filterRoutingBox.comboBox.addItemList({"Off", "Pre", "Post"}, 1);

  driveAttachment =
    std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                       "Drive",
                                       driveKnob.slider);
  mixAttachment =
    std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                       "Mix",
                                       mixKnob.slider);
  distTypeAttachment =
    std::make_unique<ComboBoxAttachment>(audioProcessor.apvts,
                                         "Type",
                                         distTypeBox.comboBox);

  cutoffAttachment =
    std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                       "Filter Cutoff",
                                       cutoffKnob.slider);
  qAttachment =
    std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                       "Filter Q",
                                       qKnob.slider);
  filterRoutingAttachment =
    std::make_unique<ComboBoxAttachment>(audioProcessor.apvts,
                                         "Filter Routing",
                                         filterRoutingBox.comboBox);

  setSize(620, 440);
}

SkuxAudioProcessorEditor::~SkuxAudioProcessorEditor()
{
  setLookAndFeel(nullptr);
}

void SkuxAudioProcessorEditor::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colour(0xff0f0f23));

  auto bounds = getLocalBounds();

  const int scopeBottom = 180 + 10;
  g.setColour(juce::Colours::white.withAlpha(0.08f));
  g.drawHorizontalLine(scopeBottom, 10.f, static_cast<float>(bounds.getWidth() - 10));

  const int controlsTop = scopeBottom + 6;
  const int controlsBottom = bounds.getHeight() - 10;
  const float centreX = static_cast<float>(bounds.getWidth()) / 2.f;
  g.drawLine(centreX, static_cast<float>(controlsTop),
             centreX, static_cast<float>(controlsBottom), 1.f);
}

void SkuxAudioProcessorEditor::resized()
{
  auto bounds = getLocalBounds().reduced(10);

  oscilloscope.setBounds(bounds.removeFromTop(180));
  bounds.removeFromTop(12);

  auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2);
  auto rightHalf = bounds;

  {
    auto area = leftHalf.reduced(6, 0);
    distortionSectionLabel.setBounds(area.removeFromTop(20));
    area.removeFromTop(4);

    const int knobW = area.getWidth() / 3;
    auto row = area;

    driveKnob.setBounds(row.removeFromLeft(knobW));
    mixKnob.setBounds(row.removeFromLeft(knobW));
    distTypeBox.setBounds(row);
  }
  
  {
    auto area = rightHalf.reduced(6, 0);
    filterSectionLabel.setBounds(area.removeFromTop(20));
    area.removeFromTop(4);

    const int knobW = area.getWidth() / 3;
    auto row = area;

    cutoffKnob.setBounds(row.removeFromLeft(knobW));
    qKnob.setBounds(row.removeFromLeft(knobW));
    filterRoutingBox.setBounds(row);
  }
}
