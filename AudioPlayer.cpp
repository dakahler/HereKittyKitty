// 
// 
// 

#include "AudioPlayer.h"
#include <assert.h>

AudioPlayer::AudioPlayer(const Tune& tune, int pin)
	: Timer(1, NULL, 1, false), m_tune(tune), m_pin(pin)
{

}

AudioPlayer::Tune::Tune(const Note* const notes, int numNotes)
	: m_notes(notes), m_numNotes(numNotes)
{
	assert(notes != NULL);
	assert(m_numNotes > 0);

	m_currentNote = 0;
}

void AudioPlayer::Play()
{
	m_tune.Reset();
	this->FireNow();
}

void AudioPlayer::Tune::Reset()
{
	m_currentNote = 0;
}

const AudioPlayer::Note& AudioPlayer::Tune::PopNote()
{
	return m_notes[m_currentNote++];
}

int AudioPlayer::Tune::NotesRemaining() const
{
	return m_numNotes - m_currentNote;
}

void AudioPlayer::CallCallback()
{
	if (m_tune.NotesRemaining() == 0)
		return;

	const Note& currentNote = m_tune.PopNote();

	// to calculate the note duration, take one second
	// divided by the note type.
	//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
	int noteDuration = 1000 / currentNote.Duration;
	noTone(m_pin);
	tone(m_pin, currentNote._Note, noteDuration);

	int pauseBetweenNotesMs = noteDuration * 1.30;

	this->m_intervalMs = currentNote.Duration + pauseBetweenNotesMs;
	this->Restart();
}
