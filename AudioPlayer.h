// AudioPlayer.h

#ifndef _AUDIOPLAYER_h
#define _AUDIOPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timer.h"
#include "Pitches.h"

class AudioPlayer : public Timer
{
public:
	struct Note
	{
		Note(Notes note, unsigned long duration) : _Note(note), Duration(duration) { }

		Notes _Note;
		unsigned long Duration;
	};

	class Tune
	{
	public:
		Tune(const Note* const notes, int numNotes);

		void Reset();
		const Note& PopNote();
		int NotesRemaining() const;

	private:
		const Note* const m_notes;
		int m_numNotes;
		int m_currentNote;
	};

	AudioPlayer(Tune& tune, int pin);
	void Play();

private:

	void CallCallback() override;

	Tune& m_tune;
	int m_pin;

};

#endif

