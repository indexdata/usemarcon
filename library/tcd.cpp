/*
 *  USEMARCON Software - Command Line version
 *  Copyright The British Library, The USEMarcon Consortium, 1995-2000
 *  Adapted by Crossnet Systems Limited - British Library Contract No. BSDS 851
 *
 *  Adapted by ATP Library Systems Ltd, Finland, 2002-2003
 *  Adapted by The National Library of Finland, 2004-2011
 *
 *  File:  tcd.cpp
 *
 *  implements a class to manage MARC record control data
 *
 */

#include <stdlib.h>
#include <ctype.h>
#include "tcd.h"
#include "statemanager.h"
#include "tools.h"


///////////////////////////////////////////////////////////////////////////////
//
// TCD
//
///////////////////////////////////////////////////////////////////////////////
TCD::TCD(TStateManager *StateManager)
{
    mStateManager     = StateManager;
    _IN                     = 0;
    *itsTag                 = '\0';
    itsSubfield[0]          = '\0';
    itsSubfield[1]          = '\0';
    itsOccurrenceNumber      = 0;
    itsTagOccurrenceNumber   = 0;
    itsSubOccurrenceNumber   = 0;
    itsPrevious         = NULL;
    itsNext             = NULL;
    itsBeginning         = 0;
    itsEnd                  = 0;
    itsTagContainsWildcard = false;
    itsTagIsWildcard = false;
    itsSubfieldContainsWildcard = false;
}

///////////////////////////////////////////////////////////////////////////////
//
// TCD : Copie constructeur a partir d'un TypeCD
//
///////////////////////////////////////////////////////////////////////////////
TCD::TCD(TypeCD* atcd, TStateManager *StateManager)
{
    mStateManager = StateManager;
    SetTag(atcd->Field);
    SetSubfield(atcd->SubField);
    itsOccurrenceNumber  = 0;
    itsTagOccurrenceNumber   = atcd->nt;
    itsSubOccurrenceNumber   = atcd->ns;
    itsPrevious = NULL;
    itsNext     = NULL;
    SetFixedPos(atcd->Fixed.str());
}


///////////////////////////////////////////////////////////////////////////////
//
// TCD : copie constructeur recursif
//
///////////////////////////////////////////////////////////////////////////////
TCD::TCD(TCD *aCD)
{
    mStateManager = aCD->mStateManager;
    _IN=aCD->GetIN();
    SetTag(aCD->GetTag());
    SetSubfield(aCD->GetSubfield());
    itsOccurrenceNumber=aCD->GetOccurrenceNumber();
    itsTagOccurrenceNumber=aCD->GetTagOccurrenceNumber();
    itsSubOccurrenceNumber=aCD->GetSubOccurrenceNumber();
    itsBeginning=aCD->GetBeginning();
    itsEnd=aCD->GetEnd();
    if (aCD->GetNext())
    {
        itsNext = (TCD *)new TCD(aCD->GetNext());
        itsNext->SetPrevious(this);
    }
    else
        itsNext = NULL;
    itsPrevious = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// ~TCD
//
///////////////////////////////////////////////////////////////////////////////
TCD::~TCD()
{
}

///////////////////////////////////////////////////////////////////////////////
//
// SetFixedPos
//
///////////////////////////////////////////////////////////////////////////////
void TCD::SetFixedPos(const char *a_str)
{
    if (!a_str)
    {
        SetBeginning(0);
        SetEnd(0);
        return;
    }

    typestr str = a_str;
    str.trim();
    // Remove all spaces
    if (str.is_empty())
    {
        SetBeginning(0);
        SetEnd(0);
        return;
    }

    // Test if the string is a Fixed position and returns if not
    const char *pt = str.cstr();
    if (*pt != '/')
        return;

    ++pt; // skip the '/' character
    const char *pt2 = pt;
    while (*pt2 && *pt2 != '/')
        pt2++;

    if (pt2 > pt)
    {
        // The Fixed position is not empty
        typestr tmp;
        tmp.str(pt, pt2 - pt);
        char *p = strchr(tmp.str(), '-');
        if (p)
        {
            if (p == tmp.str())
            {
                if (*(tmp.str() + 1) != '\0')
                {
                    SetBeginning(1);
                    SetEnd(atoi(++p));
                }
            }
            else if (*(p + 1) == '\0')
            {
                SetEnd(0);
                *p = '\0';
                SetBeginning(atoi(tmp.str()));
            }
            else
            {
                SetEnd(atoi(p + 1));
                *p = '\0';
                SetBeginning(atoi(tmp.str()));
            }
        }
        else
        {
            SetBeginning(atoi(tmp.str()));
            SetEnd(itsBeginning);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// FromString
//
///////////////////////////////////////////////////////////////////////////////
int TCD::FromString(const char *aString, const TCD *Last, int InputOrOutput)
{
    size_t StrLen, CurrentPos, RealPos=0;
    bool IsOccTagPresent=false, IsOccSubPresent=false;
    typestr str = aString;

    // TODO: make this function beautiful

    // All spaces are removed
    str.trim();
    StrLen = str.length();
    const char* String = str.cstr();
    
    // Search for the "Send to Input" character for the output CD
    SetIN(0);
    if (InputOrOutput==OUTPUT)
    {
        if (*String=='<')
        {
            SetIN(1);
            String++;
            StrLen--;
        }
    }

    // Try to read a Tag
    for (CurrentPos=RealPos; CurrentPos<StrLen && CurrentPos<3 && (isalnum(String[CurrentPos]) || String[CurrentPos]=='?'); )
        CurrentPos++;
    if (CurrentPos-RealPos==3)
        // This is the Tag
    {
        typestr tmp;
        tmp.str(&String[RealPos], CurrentPos - RealPos);
        SetTag(tmp.str());
        RealPos = CurrentPos;
        // Search for the Occurence Tag
        if (String[CurrentPos]=='(')
            // This is the Occurence Tag
        {
            // Try to read the occurence Tag
            RealPos++;
            for (CurrentPos=RealPos; CurrentPos<StrLen && String[CurrentPos]!=')'; )
                CurrentPos++;
            if (CurrentPos-RealPos)
                // the occurence Tag is not empty
            {
                IsOccTagPresent=true;
                tmp.str(&String[RealPos], CurrentPos - RealPos);
                ToUpperCase(tmp.str());
                if (!strcmp(tmp.str(), "N"))
                    SetTagOccurrenceNumber(CD_N);
                else if (!strcmp(tmp.str(), "NT"))
                    SetTagOccurrenceNumber(CD_NT);
                else if (!strcmp(tmp.str(), "NS"))
                    SetTagOccurrenceNumber(CD_NS);
                else if (!strcmp(tmp.str(), "NTO"))
                    SetTagOccurrenceNumber(CD_NTO);
                else if (!strcmp(tmp.str(), "NO"))
                    SetTagOccurrenceNumber(CD_NO);
                else if (!strcmp(tmp.str(), "NEW"))
                    SetTagOccurrenceNumber(CD_NEW);
                else if (!strcmp(tmp.str(), "NEWEST"))
                    SetTagOccurrenceNumber(CD_NEWEST);
                else 
                    SetTagOccurrenceNumber(atoi(tmp.str()));
            }
            else
                // the occurrence Tag is empty
                SetTagOccurrenceNumber(CD_NT);
            CurrentPos++;
            RealPos=CurrentPos;
        }
        else
            // there is no occurrence Tag
            SetTagOccurrenceNumber(CD_NT);
    }
    else
    {
        // There is no Tag ==> set the current Tag with the last tag
        if (Last)
        {
            IsOccTagPresent=true;
            SetTag(Last->GetTag());
            SetSubfield(Last->GetSubfield());
            SetTagOccurrenceNumber(Last->GetTagOccurrenceNumber());
        }
        else
            return 5200;
    }

    switch(String[RealPos])
    {
    case '$' :
    case 'I' :
        // This is SS
        // Try to read SS
        for (CurrentPos=RealPos+1; CurrentPos<StrLen && String[CurrentPos]!='(' && String[CurrentPos]!='/'; )
            CurrentPos++;
        if (CurrentPos - RealPos == 2)
            // this is a SS
        {
            typestr tmp;
            tmp.str(&String[RealPos], 2);
            SetSubfield(tmp.str());
        }
        else
            // Invalid SS
            return 5203;
        RealPos=CurrentPos;
        // Search for the Occurrence Sub
        if (String[RealPos]=='(')
            // There is an occurrence Sub
        {
            RealPos++;
            // Try to read the occurrence Sub
            for (CurrentPos=RealPos; CurrentPos<StrLen && String[CurrentPos]!=')'; )
                CurrentPos++;
            if (CurrentPos-RealPos)
                // the occurence Sub is no empty
            {
                IsOccSubPresent=true;
                typestr tmp;
                tmp.str(&String[RealPos], CurrentPos - RealPos);
                ToUpperCase(tmp.str());
                if (!strcmp(tmp.str(), "N"))
                    SetSubOccurrenceNumber(CD_N);
                else if (!strcmp(tmp.str(), "NT"))
                    SetSubOccurrenceNumber(CD_NT);
                else if (!strcmp(tmp.str(), "NS"))
                    SetSubOccurrenceNumber(CD_NS);
                else if (!strcmp(tmp.str(), "NSO"))
                    SetSubOccurrenceNumber(CD_NSO);
                else if (!strcmp(tmp.str(), "NO"))
                    SetSubOccurrenceNumber(CD_NO);
                else
                    SetSubOccurrenceNumber(atoi(tmp.str()));
            }
            else
                // the occurence Sub is empty
                SetSubOccurrenceNumber(CD_NS);
            CurrentPos++;
            RealPos=CurrentPos;
        }

    case '/' :
        if (String[RealPos]!='/') break;
        SetFixedPos(&String[RealPos]);
        break;

    case 0: // empty string
        break;

    default: // invalid character
        return 5212;
        break;
    }

    if (InputOrOutput==INPUT)
    {
        // No occurence N, NT etc. ( < 0) is allowed in Input CDs
        if (GetTagOccurrenceNumber() < 0)
            SetTagOccurrenceNumber(0);
        if (GetSubOccurrenceNumber() < 0)
            SetSubOccurrenceNumber(0);
        SetOccurrenceNumber(0);
    }
    else
    {
        if (!IsOccTagPresent && !IsOccSubPresent)
            // Tag Ocurence and Sub Occurence are respectively set to nt and ns if there is no defined occurence
        {
            SetTagOccurrenceNumber(CD_NT);
            SetSubOccurrenceNumber(CD_NS);
        }
        else
        {
            if (IsOccTagPresent && !IsOccSubPresent)
                SetSubOccurrenceNumber(1);
            if (!IsOccTagPresent && IsOccSubPresent)
                SetTagOccurrenceNumber(1);
        }
    }
    if (!*itsSubfield)
        SetSubOccurrenceNumber(0);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// ToString
//
///////////////////////////////////////////////////////////////////////////////
bool TCD::ToString(typestr & a_string, int InputOrOutput)
{
    int CurrentPos=0;
    int Occurrence;

    bool        bIsOccTagNT=false;
    bool        bIsOccSubNS=false;
    char            szOccTag[10];
    char            szOccSub[10];

    *szOccTag = *szOccSub = '\0';

    a_string = "";

    // Le Tag est obligatoire
    if (!*GetTag())
        return false;

    // TTT[(n1)]SS[(n2)][/p1-p2/]

    // Ecriture du caractere "Send to Input CD" si n�cessaire devant le Tag de sortie
    if (InputOrOutput == OUTPUT && GetIN())
        a_string += '<';

    // TTT
    a_string += GetTag();

    // [(n1)]
    Occurrence = GetTagOccurrenceNumber();
    switch(Occurrence)
    {
    case CD_N  : strcpy(szOccTag,"(n)");                break;
    case CD_NS : strcpy(szOccTag,"(ns)");               break;
    case CD_NTO: strcpy(szOccTag,"(nto)");              break;
    case CD_NO : strcpy(szOccTag,"(no)");               break;
    case CD_NEW: strcpy(szOccTag,"(new)");              break;
    case CD_NEWEST: strcpy(szOccTag,"(newest)");        break;
    case CD_NT : bIsOccTagNT=true;
    case 1     :
    case 0     : strcpy(szOccTag,"");                   break;
    default    : sprintf(szOccTag,"(%d)",Occurrence);   break;
    }

    const char *subfield = GetSubfield();
    if (*subfield)
    {
        // [(n2)]
        Occurrence=GetSubOccurrenceNumber();
        switch(Occurrence)
        {
        case CD_N  : strcpy(szOccSub,"(n)");            break;
        case CD_NT : strcpy(szOccSub,"(nt)");           break;
        case CD_NSO: strcpy(szOccSub,"(nso)");          break;
        case CD_NO : strcpy(szOccSub,"(no)");           break;
        case CD_NEW: strcpy(szOccSub,"(new)");          break;
        case CD_NEWEST: strcpy(szOccSub,"(newest)");    break;
        case CD_NS : bIsOccSubNS=true;
        case 1     :
        case 0     : strcpy(szOccSub,"");               break;
        default    : sprintf(szOccSub,"(%d)",Occurrence); break;
        }

        if (bIsOccTagNT && bIsOccSubNS)
            // OccTag et OccSub sont les valeurs par d�faut
            // ==> pas d'affichage de ces occurences
        {
            a_string += subfield;
        }
        else if (bIsOccTagNT && !bIsOccSubNS)
        {
            // OccTag est la valeur par d�faut mais pas OccSub
            // ==> Affichage de OccTag avec '(nt)' et de OccSub ( sauf si OccSub est � 1 )
            a_string += "(nt)";
            a_string += subfield;
            if (*szOccSub != '1')
                a_string += szOccSub;
        }
        else if (!bIsOccTagNT && bIsOccSubNS)
        {
            // OccSub est la valeur par d�faut mais pas OccTag
            // ==> Affichage de OccTag ( sauf si OccTag est � 1 ) et de OccSub avec '(ns)'
            if (*szOccTag != '1')
                a_string += szOccTag;
            a_string += subfield;
            a_string += "(ns)";
        }
        else
        {
            // OccTag et OccSub ne sont pas les valeurs par d�faut
            // ==> Affichage de OccTag et de OccSub
            a_string += szOccTag;
            a_string += subfield;
            a_string += szOccSub;
        }
    }

    if (itsBeginning)
    {
        char beginning[33];
        sprintf(beginning, "%d", itsBeginning);
        char end[33];
        sprintf(end, "%d", itsEnd);
        a_string += '/';
        a_string += beginning;
        if (itsBeginning != itsEnd)
        {
            a_string += '-';
            a_string += end;
        }
        a_string += '/';
    }

    return true;
}

void TCD::ReplaceWildcards(const char *field, const char *subfield)
{
    if (itsTagContainsWildcard)
    {
        if (itsTag[0] == '?')
            itsTag[0] = field[0];
        if (itsTag[1] == '?')
            itsTag[1] = field[1];
        if (itsTag[2] == '?')
            itsTag[2] = field[2];
    }

    if (itsSubfieldContainsWildcard)
    {
        if (itsSubfield[0] == '?')
            itsSubfield[0] = subfield[0];
        if (itsSubfield[1] == '?')
            itsSubfield[1] = subfield[1];
    }
}

void TCD::update_tag_wildcard()
{
    itsTagContainsWildcard = itsTag[0] == '?' || itsTag[1] == '?' || itsTag[2] == '?';

    if (itsTagContainsWildcard)
        itsTagIsWildcard = itsTag[0] == '?' && itsTag[1] == '?' && itsTag[2] == '?';
    else
        itsTagIsWildcard = false;
}

void TCD::update_subfield_wildcard()
{
    itsSubfieldContainsWildcard = itsSubfield[0] == '?' || itsSubfield[1] == '?';
}

void TCD::SetSubfield(const char *aSubfield)
{ 
    memcpy(itsSubfield, aSubfield, 2); 
    itsSubfield[2]=0;
    if (itsSubfield[1] && mStateManager->GetConvertSubfieldCodesToLowercase())
    {
        itsSubfield[1] = tolower(itsSubfield[1]);
    }
    update_subfield_wildcard(); 
}
