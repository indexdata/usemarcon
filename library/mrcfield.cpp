/*
 *  USEMARCON Software - Command Line version
 *  Copyright The British Library, The USEMarcon Consortium, 1995-2000
 *  Adapted by Crossnet Systems Limited - British Library Contract No. BSDS 851
 *
 *  Adapted by ATP Library Systems Ltd, Finland, 2002-2003
 *  Adapted by The National Library of Finland, 2004-2011
 *
 *  File:  mrcfield.cpp
 *
 *  implements a class to manage MARC record fields
 *
 */

#include <stdlib.h>
#include <ctype.h>

#include "mrcfield.h"
#include "statemanager.h"

///////////////////////////////////////////////////////////////////////////////
//
// TMarcField
//
///////////////////////////////////////////////////////////////////////////////
TMarcField::TMarcField(void)
{
    *itsTag         = 0;
    itsIndicators[0]    = 0;
    itsIndicators[1]    = 0;
    itsIndicators[2]    = 0;
    itsNextField        = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// TMarcField : copie constructeur recursif
//
///////////////////////////////////////////////////////////////////////////////
TMarcField::TMarcField(TMarcField *aField)
{
    strcpy(itsTag, aField->GetTag());
    itsIndicators[0] = aField->GetI1();
    itsIndicators[1] = aField->GetI2();
    itsIndicators[2] = '\0';
    SetLib1(aField->GetLib1());
    SetLib2(aField->GetLib2());
    SetScript(aField->GetScript());
    if (aField->GetNextField())
        itsNextField = new TMarcField(aField->GetNextField());
    else
        itsNextField = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// ~TMarcField
//
///////////////////////////////////////////////////////////////////////////////
TMarcField::~TMarcField(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//
// NextSubField
//
///////////////////////////////////////////////////////////////////////////////
int TMarcField::NextSubField(int *_Position, char* _SubField)
{
    short       iIndice=0;

    if (!itsLib.str() || itsLib.str()[*_Position] == '\0')
        return 0;

    if (*_SubField!='*')
    {
        while (((itsLib.str()[*_Position]!=31) || (itsLib.str()[(*_Position)+1]!=*_SubField)) && (itsLib.str()[(*_Position)+1]!=0))
            (*_Position)++;
        if (!itsLib.str()[(*_Position)+1])
            return 0;
        if (itsLib.str()[(*_Position)+1]!=*_SubField)
            return 0;
        else
        {
            if (itsLib.str()[(*_Position)+2])
                (*_Position)+=2;
            else
                return 0;
            while ((itsLib.str()[*_Position]!=31) && (itsLib.str()[*_Position]!=0))
                (*_Position)++;
            return 1;
        }
    }
    else
    {
        while ((itsLib.str()[*_Position]!=31) && (itsLib.str()[*_Position+1]!=0))
            (*_Position)++;
        if (itsLib.str()[*_Position]!=31)
            return 0;
        else
        {
            if (!itsLib.str()[(*_Position)+1])
                return 0;
            *_SubField=itsLib.str()[*_Position+1];
            if (itsLib.str()[(*_Position)+2])
                (*_Position)+=2;
            else
                return 0;
            while ((itsLib.str()[*_Position]!=31) && (itsLib.str()[*_Position]!=0))
                (*_Position)++;
            return 1;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// SetTag
//
///////////////////////////////////////////////////////////////////////////////
int TMarcField::SetTag(const char *aTagString)
{
    size_t len = strlen(aTagString);
    if (len > 3)
        return 1;

    int retval = 0;
    for (int i = 0; i < 3; i++)
    {
        if (!isalnum(aTagString[i]))
        {
            retval = 1;
            break;
        }
    }

    // Make sure tag is always three characters, prepend zeros as necessary
    char filler[4] = "000";
    filler[3 - len] = '\0';
    sprintf(itsTag, "%s%s", filler, aTagString);
    return retval;
}

int TMarcField::SetTag(int aTagNumber)
{
    if ((aTagNumber<1 ) || (aTagNumber>999))
        return 1;

    sprintf(itsTag,"%03d",aTagNumber);
    return 0;
};

int TMarcField::SetIndicators(const char *theIndicators)
{
    size_t len = strlen(theIndicators);
    if (len != 2 && len != 0)
        return 1;
    strcpy(itsIndicators, theIndicators);
    return 0;
}

void TMarcField::SetLib1(const char *aLib)
{
    if (!aLib || !*aLib)
        return;

    itsLib.str(aLib);
    return;
}

void TMarcField::SetLib1(const char *aLib, unsigned int aSize)
{
    itsLib.str(aLib, aSize);
    return;
}

void TMarcField::SetLib2(const char *aLib)
{
    if (!aLib || !*aLib)
        return;

    itsLib.s2.str(aLib);
    return;
}

void TMarcField::SetLib2(const char *aLib, unsigned int aSize)
{
    itsLib.s2.str(aLib, aSize);
    return;
}

void TMarcField::SetScript(const char *aScript)
{
    if (!aScript || !*aScript)
        return;

    itsLib.set_script(aScript);
    return;
}
