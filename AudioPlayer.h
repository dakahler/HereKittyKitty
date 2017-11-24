#ifndef _AUDIOPLAYER_h
#define _AUDIOPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timer.h"
#include "Pitches.h"
#include <assert.h>

struct Note
{
	Note(Notes note, unsigned long duration) : _Note(note), Duration(duration) { }

	Notes _Note;
	unsigned long Duration;
};

class Tune
{
public:
	Tune(const Note* const notes, int numNotes)
		: m_notes(notes), m_numNotes(numNotes)
	{
		assert(notes != NULL);
		assert(m_numNotes > 0);

		m_currentNote = 0;
	}

	void Reset()
	{
		m_currentNote = 0;
	}

	const Note& PopNote()
	{
		return m_notes[m_currentNote++];
	}

	int NotesRemaining() const
	{
		return m_numNotes - m_currentNote;
	}

private:
	const Note* const m_notes;
	int m_numNotes;
	int m_currentNote;
};

class AudioPlayer : public Timer<AudioPlayer>
{
public:
	AudioPlayer(Tune& tune, int pin)
		: Timer<AudioPlayer>(1, MethodSlot<AudioPlayer, const Timer<AudioPlayer>&>(this, &AudioPlayer::Callback), 1, false), m_tune(tune), m_pin(pin)
	{

	}

	void Play()
	{
		m_tune.Reset();
		this->FireNow();
	}

private:

	void Callback(const Timer<AudioPlayer>& timer)
	{
		if (m_tune.NotesRemaining() == 0)
			return;

		const Note& currentNote = m_tune.PopNote();

		// to calculate the note duration, take one second
		// divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		int noteDuration = 1000 / currentNote.Duration;
		//noTone(m_pin);
		//tone(m_pin, currentNote._Note, noteDuration);

		int pauseBetweenNotesMs = noteDuration * 1.30;

		this->m_intervalMs = currentNote.Duration + pauseBetweenNotesMs;
		this->Restart();
	}

	Tune& m_tune;
	int m_pin;

};

#endif

