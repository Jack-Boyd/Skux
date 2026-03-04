#pragma once
#include <array>

template <size_t BlockSize, int NumBlocks>
class ScopeDataQueue
{
public:
  ScopeDataQueue() = default;

  void push(const float* data, size_t numSamples)
  {
    for (size_t i = 0; i < numSamples; ++i) {
      currentBlock[sampleIndex++] = data[i];
      if (sampleIndex == BlockSize) {
        int start1, size1, start2, size2;
        fifo.prepareToWrite(1, start1, size1, start2, size2);

        if (size1 > 0) {
          std::copy(currentBlock.begin(),
                    currentBlock.end(),
                    blocks[static_cast<size_t>(start1)].begin());
        }

        fifo.finishedWrite(size1 + size2);
        sampleIndex = 0;
      }
    }
  }

  bool pull(std::array<float, BlockSize>& outBlock)
  {
    bool hasNewData = false;

    int numReady = fifo.getNumReady();

    while (numReady > 0) {
      int start1, size1, start2, size2;
      fifo.prepareToRead(1, start1, size1, start2, size2);

      if (size1 > 0) {
        std::copy(blocks[static_cast<size_t>(start1)].begin(),
                  blocks[static_cast<size_t>(start1)].end(),
                  outBlock.begin());
        hasNewData = true;
      }

      fifo.finishedRead(size1 + size2);
      numReady = fifo.getNumReady();
    }

    return hasNewData;
  }

private:
  juce::AbstractFifo fifo{ NumBlocks };
  std::array<std::array<float, BlockSize>, NumBlocks> blocks;
  std::array<float, BlockSize> currentBlock;
  size_t sampleIndex = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeDataQueue)
};
