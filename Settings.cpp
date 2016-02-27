/*Anime Stats
Copyright (C) 2016 Thomas Sweeney
This file is part of Anime Stats.
Anime Stats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
Anime Stats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <wx/xml/xml.h>
#include "Settings.hpp"

//start searching for siblings from hint
static wxXmlNode* findChildWithValue(wxXmlNode* node, const wxString& val, wxXmlNode* hint = nullptr)
{
    wxXmlNode* child;
    if(!hint)
        child = node->GetChildren();
    else
        child = hint->GetNext();
    if(!child)
        return nullptr;
    //loop until we've gone full circle in case of a hint
    do{
        do{
            auto debug = std::string(child->GetNodeContent().utf8_str());
            if(!child->GetName().compare(val))
                return child;
            if(child == hint) //we've now completed the loop and found nothing
                return nullptr;
        }while((child = child->GetNext()));
        child = node->GetChildren();
    }while(hint);
    return nullptr;
}

//we construct cellColors this way instead of doing it all in one go because
//we don't want to rely on the columns existing or being in a certain order
Settings::Settings()
    : cellColors(col::NUM_COLS - col::FIRST_VISIBLE_COL)
{
    //wxColor interprets hex as BGR instead of RGB for some reason
    cellColors[col::RATING - col::FIRST_VISIBLE_COL] = std::vector<long> {-1, 0x6B69F8, 0x84EBFF, 0x7BBE63};
    cellColors[col::WATCHED_STATUS - col::FIRST_VISIBLE_COL] = std::vector<long> {-1, 0xFFFFFF, 0xFFC271, 0x94FF3B, 0x83B8F9, 0x7D7DFF};
}

Settings::Settings(const wxString& fn)
{
    wxXmlDocument doc;
    if(!doc.Load(fn))
        throw SettingsLoadException();
    auto root = doc.GetRoot();
    if(root){
        //default DB file
        auto topNode = findChildWithValue(root, "defaultDb");
        if(topNode)
            defaultDb = std::string(topNode->GetNodeContent().utf8_str());

        //default DB ask
        topNode = findChildWithValue(root, "defaultDbAsk", topNode);
        if(topNode){
            long temp;
            if(topNode->GetNodeContent().ToLong(&temp))
                defaultDbAsk = temp;
        }

        //sort by pronunciation?
        topNode = findChildWithValue(root, "sortingByPronunciation", topNode);
        if(topNode){
            long temp;
            if(topNode->GetNodeContent().ToLong(&temp))
                sortingByPronunciation = temp;
        }

        //column sizes and colors
        topNode = findChildWithValue(root, "cols", topNode);
        if(topNode){
            wxXmlNode* colNode = nullptr;
            for(int i = 0; i < col::NUM_COLS - col::FIRST_VISIBLE_COL; ++i){
                cellColors.emplace_back(); //we want cellColors to have a spot for all the columns no matter what
                colNode = findChildWithValue(topNode, wxString::Format("col%i", i), colNode);
                if(colNode){
                    //sizes
                    auto sizeNode = findChildWithValue(colNode, "size");
                    if(sizeNode){
                        long temp;
                        if(!sizeNode->GetNodeContent().ToLong(&temp))
                            temp = -1;
                        colSizes.push_back(temp);
                    }

                    //colors
                    auto colorsNode = findChildWithValue(colNode, "colors", sizeNode);
                    if(colorsNode){
                        auto child = colorsNode->GetChildren();
                        if(child)
                            do{
                                long temp;
                                if(child->GetNodeContent().ToLong(&temp))
                                    cellColors.back().push_back(temp);
                            }while((child = child->GetNext()));
                    }
                        
                }
            }
        }
    }
    else
        throw SettingsLoadException();
}

//have to attach the child nodes in backwards order because wxWidgets is dumb as hell and their documentation is completely wrong
void Settings::Save(const wxString& fn)
{
    //root
    wxXmlDocument doc;
    auto rootNode = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "settings");
    doc.SetRoot(rootNode);

    //cols
    auto colsNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, "cols");
    for(int i = cellColors.size() - 1; i >= 0; --i){
        auto iNode = new wxXmlNode(colsNode, wxXML_ELEMENT_NODE, wxString::Format("col%i", i));
        //col colors
        if(cellColors[i].size()){
            auto colorsNode = new wxXmlNode(iNode, wxXML_ELEMENT_NODE, "colors");
            colorsNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", ""));
            for(auto it = cellColors[i].rbegin(); it != cellColors[i].rend(); ++it){
                auto colorsChildNode = new wxXmlNode(colorsNode, wxXML_ELEMENT_NODE, "color");
                colorsChildNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format("%i", *it)));
            }
        }
        //col size
        if(colSizes.size()){ //the program should be able to handle colSizes not existing and so we don't need to write values for them here.
            auto sizeNode = new wxXmlNode(iNode, wxXML_ELEMENT_NODE, "size");
            sizeNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format("%i", colSizes[i])));
        }
    }
    //sortingByPronunciation
    auto sortingByPronunciationNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, "sortingByPronunciation");
    sortingByPronunciationNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "",
            wxString::Format("%i", sortingByPronunciation)));
    //defaultDbAsk
        auto defaultDbAskNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, "defaultDbAsk");
        defaultDbAskNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format("%i", defaultDbAsk)));
    //defaultDb
    auto defaultDbNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, "defaultDb");
    defaultDbNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", defaultDb));
    if(!doc.Save(fn))
        throw SettingsSaveException();
}
