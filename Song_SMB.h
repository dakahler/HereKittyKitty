#pragma once

Note song_smb_notes[] =
{
	Note(Notes::NOTE_E7, 12), Note(Notes::NOTE_E7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_E7, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_C7, 12), Note(Notes::NOTE_E7, 12), Note(Notes::NOTE_SILENCE, 12),
	Note(Notes::NOTE_G7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12),
	Note(Notes::NOTE_G6, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12),

	Note(Notes::NOTE_C7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_G6, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_E6, 12), Note(Notes::NOTE_SILENCE, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_A6, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_B6, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_AS6, 12), Note(Notes::NOTE_A6, 12), Note(Notes::NOTE_SILENCE, 12),

	Note(Notes::NOTE_G6, 9), Note(Notes::NOTE_E7, 9), Note(Notes::NOTE_G7, 9),
	Note(Notes::NOTE_A7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_F7, 12), Note(Notes::NOTE_G7, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_E7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_C7, 12),
	Note(Notes::NOTE_D7, 12), Note(Notes::NOTE_B6, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12),

	Note(Notes::NOTE_C7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_G6, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_E6, 12), Note(Notes::NOTE_SILENCE, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_A6, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_B6, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_AS6, 12), Note(Notes::NOTE_A6, 12), Note(Notes::NOTE_SILENCE, 12),

	Note(Notes::NOTE_G6, 9), Note(Notes::NOTE_E7, 9), Note(Notes::NOTE_G7, 9),
	Note(Notes::NOTE_A7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_F7, 12), Note(Notes::NOTE_G7, 12),
	Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_E7, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_C7, 12),
	Note(Notes::NOTE_D7, 12), Note(Notes::NOTE_B6, 12), Note(Notes::NOTE_SILENCE, 12), Note(Notes::NOTE_SILENCE, 12),
};

Tune song_smb(song_smb_notes, sizeof(song_smb_notes) / sizeof(song_smb_notes[0]));