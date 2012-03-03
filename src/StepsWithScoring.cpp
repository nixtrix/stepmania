#include "global.h"
#include "StepsWithScoring.h"
#include "NoteDataWithScoring.h" // For things the Steps don't need to do.
#include "NoteData.h"
#include "PlayerStageStats.h"
#include "GameConstantsAndTypes.h"
#include "ThemeMetric.h"
#include "Steps.h"
#include "RageLog.h"

static ThemeMetric<TapNoteScore> MIN_SCORE_TO_MAINTAIN_COMBO( "Gameplay", "MinScoreToMaintainCombo" );

const TapNote &StepsWithScoring::LastTapNoteWithResult( const NoteData &in, unsigned row, StepsTypeCategory stc, PlayerNumber pn )
{
	int track;
	switch (stc)
	{
		case StepsTypeCategory_Couple:
		{
			const int tracksPerPlayer = in.GetNumTracks() / 2;
			int start = pn * tracksPerPlayer;
			track = NoteDataWithScoring::LastTapNoteScoreTrack(in, row, start, start + tracksPerPlayer, PLAYER_INVALID);
			break;
		}
		case StepsTypeCategory_Routine:
		{
			track = NoteDataWithScoring::LastTapNoteScoreTrack(in, row, 0, in.GetNumTracks(), pn);
			break;
		}
		default:
		{
			track = NoteDataWithScoring::LastTapNoteScoreTrack(in, row, 0, in.GetNumTracks(), PLAYER_INVALID);
		}
	}
	if( track == -1 )
		return TAP_EMPTY;

	//LOG->Trace( ssprintf("returning in.GetTapNote(iTrack=%i, iRow=%i)", iTrack, iRow) );
	return in.GetTapNote( track, row );
}

bool StepsWithScoring::IsRowCompletelyJudged( const NoteData &in, unsigned row, StepsTypeCategory stc, PlayerNumber pn )
{
	TapNoteScore tns;
	switch (stc)
	{
		case StepsTypeCategory_Couple:
		{
			const int tracksPerPlayer = in.GetNumTracks() / 2;
			int start = pn * tracksPerPlayer;
			tns = NoteDataWithScoring::MinTapNoteScore(in, row, start, start + tracksPerPlayer, PLAYER_INVALID);
			break;
		}
		case StepsTypeCategory_Routine:
		{
			tns = NoteDataWithScoring::MinTapNoteScore(in, row, 0, in.GetNumTracks(), pn);
			break;
		}
		default:
		{
			tns = NoteDataWithScoring::MinTapNoteScore(in, row);
		}
	}
	return tns >= TNS_Miss;
}

namespace // radar calculation namespace
{

// Return the ratio of actual combo to max combo.
float GetActualVoltageRadarValue(const PlayerStageStats &pss)
{
	/* STATSMAN->m_CurStageStats.iMaxCombo is unrelated to GetNumTapNotes:
	 * m_bComboContinuesBetweenSongs might be on, and the way combo is counted
	 * varies depending on the mode and score keeper. Instead, let's use the
	 * length of the longest recorded combo. This is only subtly different:
	 * it's the percent of the song the longest combo took to get. */
	const PlayerStageStats::Combo_t MaxCombo = pss.GetMaxCombo();
	float fComboPercent = SCALE( MaxCombo.m_fSizeSeconds, 0, pss.m_fLastSecond-pss.m_fFirstSecond, 0.0f, 1.0f );
	return clamp( fComboPercent, 0.0f, 1.0f );
}

// Return the ratio of actual to possible dance points.
float GetActualChaosRadarValue( const PlayerStageStats &pss )
{
	const int iPossibleDP = pss.m_iPossibleDancePoints;
	if ( iPossibleDP == 0 )
		return 1;

	const int ActualDP = pss.m_iActualDancePoints;
	return clamp( float(ActualDP)/iPossibleDP, 0.0f, 1.0f );
}

}

RadarValues StepsWithScoring::GetActualRadarValues(const Steps *in,
	const PlayerStageStats &pss,
	float fSongSeconds,
	PlayerNumber pn)
{
	RadarValues rv;
	// The for loop and the assert are used to ensure that all fields of 
	// RadarValue get set in here.
	vector<int> statResults;
	vector<float> radarResults;
	FOREACH_ENUM( RadarCategory, rc )
	{
		switch( rc )
		{
		case RadarCategory_Stream:		rv[rc] = GetActualStreamRadarValue( in, fSongSeconds );				break;
			case RadarCategory_Voltage:
			{
				rv[rc] = GetActualVoltageRadarValue(pss);
				break;
			}
		case RadarCategory_Air:			rv[rc] = GetActualAirRadarValue( in, fSongSeconds );					break;
		case RadarCategory_Freeze:		rv[rc] = GetActualFreezeRadarValue( in, fSongSeconds );				break;
			case RadarCategory_Chaos:
			{
				rv[rc] = GetActualChaosRadarValue(pss);
				break;
			}
		case RadarCategory_TapsAndHolds:	rv[rc] = (float) GetNumNWithScore( in, TNS_W4, 1 );					break;
		case RadarCategory_Jumps:		rv[rc] = (float) GetNumNWithScore( in, TNS_W4, 2 );					break;
		case RadarCategory_Holds:		rv[rc] = (float) GetNumHoldNotesWithScore( in, TapNote::hold_head_hold, HNS_Held );	break;
		case RadarCategory_Mines:		rv[rc] = (float) GetSuccessfulMines( in );						break;
		case RadarCategory_Hands:		rv[rc] = (float) GetSuccessfulHands( in );						break;
		case RadarCategory_Rolls:		rv[rc] = (float) GetNumHoldNotesWithScore( in, TapNote::hold_head_roll, HNS_Held );	break;
		case RadarCategory_Lifts:		rv[rc] = (float) GetSuccessfulLifts( in, MIN_SCORE_TO_MAINTAIN_COMBO );					break;
			case RadarCategory_Fakes:
			{
				statResults = in->GetNumFakes();
				if (statResults.size() == 1)
				{
					statResults.push_back(statResults[0]);
				}
				rv[rc] = statResults[pn];
				break;
			}
		DEFAULT_FAIL( rc );
		}
	}
}