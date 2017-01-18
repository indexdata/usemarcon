/*
 *  USEMARCON Software - Command Line version
 *  Copyright The British Library, The USEMarcon Consortium, 1995-2000
 *  Adapted by Crossnet Systems Limited - British Library Contract No. BSDS 851
 *
 *  Adapted by ATP Library Systems Ltd, Finland, 2002-2003
 *  Adapted by The National Library of Finland, 2004-2011
 *
 *  File:  ctrlfld.cpp
 *
 *  implements a class to manage MARC control fields
 *
 */

#include <ctype.h>
#include <stdlib.h>

#include "ctrlfld.h"
#include "statemanager.h"

///////////////////////////////////////////////////////////////////////////////
//
// TCtrlSubfield
//
///////////////////////////////////////////////////////////////////////////////
TCtrlSubfield::TCtrlSubfield(TStateManager *mStateManager)
{
    itsSub              = '?';
    itsSubMandatory = false;
    itsSubRepeatable    = false;
    itsSubOccurency = 0;
    itsNextSub          = NULL;
    mStateManager = mStateManager;
}

///////////////////////////////////////////////////////////////////////////////
//
// ~TCtrlSubfield
//
///////////////////////////////////////////////////////////////////////////////
TCtrlSubfield::~TCtrlSubfield(void)
{

}

///////////////////////////////////////////////////////////////////////////////
//
// TControlField
//
///////////////////////////////////////////////////////////////////////////////
TControlField::TControlField(TStateManager *StateManager)
{
    *itsTag         = 0;
    itsTagMandatory = false;
    itsTagRepeatable    = false;
    itsTagOccurrency = 0;
    itsFirstSubfield    = NULL;
    itsLastSubfield = NULL;
    itsNextTag          = NULL;
    mStateManager    = StateManager;
}

///////////////////////////////////////////////////////////////////////////////
//
// ~TControlField
//
///////////////////////////////////////////////////////////////////////////////
TControlField::~TControlField(void)
{
    DelTreeCheckSub();
}

///////////////////////////////////////////////////////////////////////////////
//
// SetFirstIndicators
//
///////////////////////////////////////////////////////////////////////////////
int TControlField::SetFirstIndicators(const char *Indicators)
{
    itsFirstIndicators.freestr();
    if (*Indicators)
        if (itsFirstIndicators.str(Indicators)==NULL)
            return mStateManager->SetError(2503,ERROR);
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetSecondIndicators
//
///////////////////////////////////////////////////////////////////////////////
int TControlField::SetSecondIndicators(const char *Indicators)
{
    itsSecondIndicators.freestr();
    if (*Indicators)
        if (itsSecondIndicators.str(Indicators)==NULL)
            return mStateManager->SetError(2504,ERROR);
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// DelTreeCheckSub
//
///////////////////////////////////////////////////////////////////////////////
void TControlField::DelTreeCheckSub(void)
{
    TCtrlSubfield   *Courant;
    TCtrlSubfield   *Suivant;

    Courant = itsFirstSubfield;
    while (Courant)
    {
        Suivant = Courant->GetNextSub();
        delete Courant;
        Courant = Suivant;
    }
    itsFirstSubfield = itsLastSubfield = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// TControlField
//
///////////////////////////////////////////////////////////////////////////////
void TControlField::ResetSubOccurency(TCtrlSubfield *StartSub)
{
    TCtrlSubfield   *Current;

    Current=StartSub;
    while (Current)
    {
        Current->SetSubOccurency(0);
        Current=Current->GetNextSub();
    }
}




