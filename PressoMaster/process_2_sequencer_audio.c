/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * process_2_sequencer_audio.c
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"


Presso_sound_TypeDef	Presso_Requested_sound[16];

Presso_sound_TypeDef	Presso_Initial_sound[] =
{
		{MIDI_NOTE_ON_FLAG,67,200},
		{MIDI_NOTE_ON_FLAG,69,200},
		{MIDI_NOTE_ON_FLAG,71,200},
		{MIDI_NOTE_ON_FLAG,74,200},
		{MIDI_NOTE_ON_FLAG,79,200},
		{MIDI_NOTE_ON_FLAG,74,200},
		{MIDI_NOTE_ON_FLAG,71,200},
		{MIDI_NOTE_ON_FLAG,69,200},
		{MIDI_NOTE_ON_FLAG,67,200},
		{MIDI_NOTE_MUTE_FLAG,0,0},
};

/*
void sound_seq_run_OLD ( void  )
{
Presso_sound_TypeDef *sound = Presso_soundseq.sound;
	if ( Presso_soundseq.state == SOUND_SEQ_IDLE )
		return;
	if ( Presso_soundseq.note_time == 0 )
	{
		Presso_soundseq.state=SOUND_SEQ_IDLE;
		return;
	}
	Presso_soundseq.note_time--;
	if ( Presso_soundseq.note_time == 0 )
	{
		if (( sound[Presso_soundseq.note_index].flags & MIDI_NOTE_OFF_FLAG) == MIDI_NOTE_OFF_FLAG)
			NoteOff( sound[Presso_soundseq.note_index].midi_note);
		Presso_soundseq.note_index++;
		if (( sound[Presso_soundseq.note_index].flags & MIDI_NOTE_ON_FLAG) == MIDI_NOTE_ON_FLAG)
		{
			Presso_soundseq.note_time = sound[Presso_soundseq.note_index].time / 10;
			NoteOn( sound[Presso_soundseq.note_index].midi_note , 100);
		}
		else
		{
			Presso_soundseq.note_time = Presso_soundseq.note_index = 0;
			Presso_soundseq.state=SOUND_SEQ_IDLE;
			AllNoteOFF();
		}
	}
}
*/

void sound_seq_run ( void  )
{
Presso_sound_TypeDef *sound;

	if ( Presso_soundseq.state == SOUND_SEQ_IDLE )
		return;
	sound = &Presso_soundseq.sound[Presso_soundseq.note_index];
	Presso_soundseq.note_time--;
	if ( Presso_soundseq.note_time == 0 )
	{
		if (( sound->flags & MIDI_NOTE_OFF_FLAG) == MIDI_NOTE_OFF_FLAG)
			NoteOff( sound->midi_note);
		Presso_soundseq.note_index++;
		if (( sound->flags & MIDI_NOTE_ON_FLAG) == MIDI_NOTE_ON_FLAG)
		{
			Presso_soundseq.note_time = sound->time / 10;
			NoteOn( sound->midi_note , 100);
		}
		else
		{
			Presso_soundseq.note_time = Presso_soundseq.note_index = 0;
			Presso_soundseq.state=SOUND_SEQ_IDLE;
			AllNoteOFF();
		}
	}

}

void sound_seq_start ( Presso_sound_TypeDef *sound )
{
	Presso_soundseq.state=SOUND_SEQ_STARTED;
	Presso_soundseq.note_index=0;
	Presso_soundseq.sound=sound;
	Presso_soundseq.note_time = sound[Presso_soundseq.note_index].time / 10;
	if ( sound[Presso_soundseq.note_index].flags )
		NoteOn( sound[Presso_soundseq.note_index].midi_note , 1);
}

Presso_ee_sound_TypeDef	Presso_sound;

void sound_play ( uint8_t sound_number )
{
uint32_t	i;
	mem_load_sound(sound_number,&Presso_sound);
	for(i=0;i<Presso_sound.sound_number_of_lines;i++)
	{
		Presso_Requested_sound[i].flags = Presso_sound.Presso_ee_sound_line[i].midi_flag;
		Presso_Requested_sound[i].midi_note = Presso_sound.Presso_ee_sound_line[i].midi_note;
		Presso_Requested_sound[i].time = Presso_sound.Presso_ee_sound_line[i].midi_time;
	}
	Presso_Requested_sound[i].flags = 0;
	Presso_Requested_sound[i].midi_note = 0;
	Presso_Requested_sound[i].time = 0;
	sound_seq_start(&Presso_Requested_sound[0]);
}

