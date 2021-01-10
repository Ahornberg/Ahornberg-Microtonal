#include <fstream>
#include <osdialog.h>
#include "MIDIOverAudioWidget.hpp"

extern std::vector<MIDIOverAudioWidget*>* MIDIOverAudioWidget::widgets;

MIDIOverAudioDriverMenuItem::MIDIOverAudioDriverMenuItem(MIDIOverAudio* midiOverAudio) {
	this->midiOverAudio = midiOverAudio;
	
	text = "MIDI over Audio driver";
	if (MIDIOverAudioDriver::driver->enabled) {
		rightText = "enabled";
	} else {
		rightText = "disabled";
	}
}

void MIDIOverAudioDriverMenuItem::onAction(const event::Action& e) {
	if (midiOverAudio) {
		if (!MIDIOverAudioDriver::driver->enabled) {
			std::string text = "After enabling the MIDI over Audio driver, you will get an annoying error message every time you close the Rack. This will not harm your patches in any way. Furthermore it is possible to disable the MIDI over Audio driver at anytime you like. Do you want to enable the MIDI over Audio driver now?";
			if (osdialog_message(OSDIALOG_INFO, OSDIALOG_OK_CANCEL, text.c_str())) {
				saveMIDIOverAudioDriverEnabled(true);
				new MIDIOverAudioDriver();
				for (auto widget : *MIDIOverAudioWidget::widgets) {
					widget->setPanel("res/MIDIOverAudioEnabled.svg");
				}
			}
		} else {
			std::string text = "You're about to disable the MIDI over Audio driver. For this to take effect, it is required to close and re-launch the Rack. Do you want to close an re-launch the Rack in oder to disable the MIDI over Audio driver now?";
			if (osdialog_message(OSDIALOG_INFO, OSDIALOG_OK_CANCEL, text.c_str())) {
				saveMIDIOverAudioDriverEnabled(false);
				APP->window->close();
			}
		}
	}
}

MIDIOverAudioWidget::MIDIOverAudioWidget(MIDIOverAudio* module) {
	init();
	setModule(module);
	if (MIDIOverAudioDriver::driver->enabled) {
		setPanel("res/MIDIOverAudioEnabled.svg");
	} else {
		setPanel("res/MIDIOverAudioDisabled.svg");
	}
	setSize(Vec(30, 380));
	setScrews(SCREW_TOP_LEFT, NO_SCREW_TOP_RIGHT, NO_SCREW_BOTTOM_LEFT, SCREW_BOTTOM_RIGHT);

	for (auto i = 0; i < MIDI_OVER_AUDIO_MAX_DEVICES; ++i) {
		addInput(createInputCentered<InPort>(Vec(15, 95 - 9 + i * 37), module, MIDIOverAudio::MIDI_OVER_AUDIO_INPUT + i));
		addParam(createParamCentered<KnobScrew>(Vec(8.3f, 77 - 9 + i * 37),   module, MIDIOverAudio::TRIM_PARAM + i));
		addChild(createLightCentered<SmallLight<RedGreenBlueLight>>(Vec(19, 77 - 9 + i * 37), module, MIDIOverAudio::RGB_LIGHT + i * 3));
	}
}

MIDIOverAudioWidget::~MIDIOverAudioWidget() {
	auto it = std::find(MIDIOverAudioWidget::widgets->begin(), MIDIOverAudioWidget::widgets->end(), this);
	if (it != MIDIOverAudioWidget::widgets->end()) {
		MIDIOverAudioWidget::widgets->erase(it);
	}
}

void MIDIOverAudioWidget::init() {
	if (MIDIOverAudioWidget::widgets == nullptr) {
		MIDIOverAudioWidget::widgets = new std::vector<MIDIOverAudioWidget*>;
	}
	MIDIOverAudioWidget::widgets->push_back(this);
}

void MIDIOverAudioWidget::appendContextMenu(Menu* menu) {
	MIDIOverAudio* midiOverAudio = dynamic_cast<MIDIOverAudio*>(this->module);
	menu->addChild(new MenuEntry);
	menu->addChild(new MIDIOverAudioDriverMenuItem(midiOverAudio));
}

Model* modelMIDIOverAudio = createModel<MIDIOverAudio, MIDIOverAudioWidget>("MIDIOverAudio");
