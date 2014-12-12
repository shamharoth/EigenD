/*
 Copyright 2012-2014 Eigenlabs Ltd.  http://www.eigenlabs.com

 This file is part of EigenD.

 EigenD is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 EigenD is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with EigenD.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __KEYTOCOURSEPANEL__
#define __KEYTOCOURSEPANEL__

#include <piw/piw_tsd.h>
#include "workspace.h"
#include "juce.h"
#include "XYToZComponent.h"
#include <list>
class XYToZComponent;

class KeyToCoursePanel  : public Component
{
public:
    KeyToCoursePanel (Atom* atom, String mapping, Component* editor);
    ~KeyToCoursePanel();
    void addElement();
    void removeElement(XYToZComponent*);
    void updateMapping();
    void displayMapping(String);
    void revertMapping(String);
    void checkRevertable();
    void clearMapping();
    void paint (Graphics& g);
    void resized();

private:
    std::list<XYToZComponent*> elements_;
    Atom* atom_;
    String mapping_;
    String makeMappingString();
    String makeDisplayedMappingString();
    Component* editor_;
    void doDisplayMapping(String);
    KeyToCoursePanel (const KeyToCoursePanel&);
    const KeyToCoursePanel& operator= (const KeyToCoursePanel&);
};


#endif 