#include "LookAndFeel.h"

LookAndFeel::LookAndFeel()
{
  setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a1a2e));
  setColour(juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha(0.2f));
  setColour(juce::ComboBox::textColourId, juce::Colour(0xff00e5ff));
  setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff00e5ff));
  setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff1a1a2e));
  setColour(juce::PopupMenu::textColourId, juce::Colours::white);
  setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff00e5ff).withAlpha(0.3f));
  setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
  
  juce::Typeface::Ptr typeface =
    juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);
  setDefaultSansSerifTypeface(typeface);
}

void LookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
  const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.f);
  const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.f;
  const auto centreX = bounds.getCentreX();
  const auto centreY = bounds.getCentreY();
  const auto rx = centreX - radius;
  const auto ry = centreY - radius;
  const auto diameter = radius * 2.f;
  const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

  g.setColour(juce::Colour(0xff1a1a2e));
  g.fillEllipse(rx, ry, diameter, diameter);

  juce::Path bgArc;
  bgArc.addCentredArc(centreX, centreY, radius - 2.f, radius - 2.f,
                      0.f, rotaryStartAngle, rotaryEndAngle, true);
  g.setColour(juce::Colours::white.withAlpha(0.1f));
  g.strokePath(bgArc, juce::PathStrokeType(3.f,
                                           juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded));

  juce::Path valueArc;
  valueArc.addCentredArc(centreX, centreY, radius - 2.f, radius - 2.f,
                         0.f, rotaryStartAngle, angle, true);
  g.setColour(juce::Colour(0xff00e5ff));
  g.strokePath(valueArc, juce::PathStrokeType(3.f,
                                              juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

  juce::Path pointer;
  const auto pointerLength = radius * 0.5f;
  const auto pointerThickness = 2.5f;
  pointer.addRectangle(-pointerThickness * 0.5f, -radius + 6.f, pointerThickness, pointerLength);
  pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
  g.setColour(juce::Colours::white);
  g.fillPath(pointer);

  const float dotSize = 4.f;
  g.setColour(juce::Colour(0xff00e5ff));
  g.fillEllipse(centreX - dotSize / 2.f, centreY - dotSize / 2.f, dotSize, dotSize);
}

void LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
  g.setColour(label.findColour(juce::Label::textColourId));
  g.setFont(label.getFont());
  g.drawFittedText(label.getText(), label.getLocalBounds(),
                   label.getJustificationType(),
                   juce::jmax(1, (int)(label.getHeight() / 12)),
                   label.getMinimumHorizontalScale());
}
