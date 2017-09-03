#include <mutex>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "core/audio/jack.hpp"
#include "core/audio/midi.hpp"
#include "core/ui/mainui.hpp"
#include "modules/tapedeck.hpp"
#include "modules/mixer.hpp"
#include "modules/super-saw-synth.hpp"
#include "modules/simple-drums.hpp"
#include "modules/drum-sampler.hpp"
#include "modules/synth-sampler.hpp"
#include "modules/nuke.hpp"
#include "core/globals.hpp"

int main(int argc, char *argv[]) {
  static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::debug, "log.txt").addAppender(&consoleAppender);
  LOGI << "LOGGING NOW";

  using namespace top1;

  midi::generateFreqTable(440);
  std::mutex mut;
  std::unique_lock lock (mut);

  Globals::drums.registerModule("Sampler", new modules::DrumSampler());
  Globals::drums.registerModule("Additive Drums", new modules::SimpleDrumsModule());

  Globals::synth.registerModule("Nuke", new modules::NukeSynth());
  Globals::synth.registerModule("Super Saw", new modules::SuperSawSynth());
  Globals::synth.registerModule("Sampler", new modules::SynthSampler());

  Globals::events.preInit.runAll();
  Globals::init();
  Globals::events.postInit.runAll();

  Globals::jackAudio.startProcess();

  Globals::notifyExit.wait(lock);

  LOGI << "Exitting";
  Globals::events.preExit.runAll();
  Globals::ui.exit();
  Globals::mixer.exit();
  Globals::tapedeck.exit();
  Globals::jackAudio.exit();
  Globals::dataFile.write();
  Globals::events.postExit.runAll();
  return 0;
}
