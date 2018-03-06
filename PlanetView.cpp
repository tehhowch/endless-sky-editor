/* PlanetView.cpp
Copyright (c) 2015 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "PlanetView.h"

#include "LandscapeView.h"
#include "Map.h"
#include "StellarObject.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QRegExp>
#include <QRegExpValidator>

#include <cmath>
#include <limits>

using namespace std;

namespace {
    double GetOptionalValue(const QString &text)
    {
        return text.isEmpty() ? numeric_limits<double>::quiet_NaN() : text.toDouble();
    }
}



PlanetView::PlanetView(Map &mapData, QWidget *parent) :
    QWidget(parent), mapData(mapData)
{
    name = new QLineEdit(this);
    connect(name, SIGNAL(editingFinished()), this, SLOT(NameChanged()));
    government = new QLineEdit(this);
    connect(government, SIGNAL(editingFinished()), this, SLOT(GovernmentChanged()));
    attributes = new QLineEdit(this);
    connect(attributes, SIGNAL(editingFinished()), this, SLOT(AttributesChanged()));

    landscape = new LandscapeView(mapData, this);
    landscape->setMinimumHeight(360);
    landscape->setMaximumHeight(360);

    description = new QPlainTextEdit(this);
    description->setTabStopWidth(20);
    description->setPlaceholderText("Add a description. Descriptions are the default visible text while landed.");
    connect(description, SIGNAL(textChanged()), this, SLOT(DescriptionChanged()));

    spaceport = new QPlainTextEdit(this);
    spaceport->setTabStopWidth(20);
    spaceport->setPlaceholderText("Optional text to be shown if the player clicks the \"Spaceport\" button.");
    connect(spaceport, SIGNAL(textChanged()), this, SLOT(SpaceportDescriptionChanged()));

    shipyard = new QLineEdit(this);
    connect(shipyard, SIGNAL(editingFinished()), this, SLOT(ShipyardChanged()));
    outfitter = new QLineEdit(this);
    connect(outfitter, SIGNAL(editingFinished()), this, SLOT(OutfitterChanged()));

    reputation = new QLineEdit(this);
    reputation->setPlaceholderText("0");
    reputation->setMaximumWidth(100);
    reputation->setValidator(new QRegExpValidator(QRegExp("-?\\d*\\.?\\d*"), reputation));
    connect(reputation, SIGNAL(editingFinished()), this, SLOT(ReputationChanged()));

    bribe = new QLineEdit(this);
    bribe->setPlaceholderText("0.01");
    bribe->setMaximumWidth(100);
    bribe->setValidator(new QRegExpValidator(QRegExp("0||0?\\.\\d*"), bribe));
    connect(bribe, SIGNAL(editingFinished()), this, SLOT(BribeChanged()));

    security = new QLineEdit(this);
    security->setPlaceholderText("0.25");
    security->setMaximumWidth(100);
    security->setValidator(new QRegExpValidator(QRegExp("0||0?\\.\\d*"), security));
    connect(security, SIGNAL(editingFinished()), this, SLOT(SecurityChanged()));

    tribute = new QLineEdit(this);
    tribute->setPlaceholderText("0");
    tribute->setMaximumWidth(100);
    tribute->setValidator(new QRegExpValidator(QRegExp("\\d*"), tribute));
    connect(tribute, SIGNAL(editingFinished()), this, SLOT(TributeChanged()));

    tributeThreshold = new QLineEdit(this);
    tributeThreshold->setPlaceholderText("4000");
    tributeThreshold->setMaximumWidth(100);
    tributeThreshold->setValidator(new QRegExpValidator(QRegExp("\\d*"), tributeThreshold));
    connect(tributeThreshold, SIGNAL(editingFinished()), this, SLOT(TributeThresholdChanged()));

    tributeFleetName = new QLineEdit(this);
    tributeFleetName->setMinimumWidth(200);
    connect(tributeFleetName, SIGNAL(editingFinished()), this, SLOT(TributeFleetNameChanged()));

    tributeFleetQuantity = new QLineEdit(this);
    tributeFleetQuantity->setPlaceholderText("0");
    tributeFleetQuantity->setMaximumWidth(100);
    tributeFleetQuantity->setValidator(new QRegExpValidator(QRegExp("\\d*"), tributeFleetQuantity));
    connect(tributeFleetQuantity, SIGNAL(editingFinished()), this, SLOT(TributeFleetQuantityChanged()));


    QGridLayout *layout = new QGridLayout(this);
    int row = 0;

    // Align the name and government in the same line.
    QWidget *nameBox = new QWidget(this);
    {
        QHBoxLayout *hLayout = new QHBoxLayout(nameBox);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->addWidget(new QLabel("Planet:", this));
        hLayout->addWidget(name);
        hLayout->addWidget(new QLabel("Government:", this));
        hLayout->addWidget(government);
        hLayout->addStretch();
    }
    layout->addWidget(nameBox, row++, 0, 1, 2);
    layout->addWidget(new QLabel("Attributes:", this), row, 0);
    layout->addWidget(attributes, row++, 1);

    layout->addWidget(landscape, row++, 0, 1, 2);

    layout->addWidget(description, row++, 0, 1, 2);

    layout->addWidget(new QLabel("Spaceport description:", this), row++, 0, 1, 2);
    layout->addWidget(spaceport, row++, 0, 1, 2);

    layout->addWidget(new QLabel("Shipyard:", this), row, 0);
    layout->addWidget(shipyard, row++, 1);

    layout->addWidget(new QLabel("Outfitter:", this), row, 0);
    layout->addWidget(outfitter, row++, 1);

    // Align landing / security controls in the same line.
    QWidget *box = new QWidget(this);
    {
        QHBoxLayout *hLayout = new QHBoxLayout(box);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->addWidget(new QLabel("Required reputation:", this));
        hLayout->addWidget(reputation);
        hLayout->addWidget(new QLabel("Bribe:", this));
        hLayout->addWidget(bribe);
        hLayout->addWidget(new QLabel("Security:", this));
        hLayout->addWidget(security);
        hLayout->addStretch();
    }
    layout->addWidget(box, row++, 0, 1, 2);

    // Align tribute controls in the same line.
    QWidget *tributeBox = new QWidget(this);
    {
        QHBoxLayout *tributeHLayout = new QHBoxLayout(tributeBox);
        tributeHLayout->setContentsMargins(0, 0, 0, 0);
        tributeHLayout->addWidget(new QLabel("Tribute:", this));
        tributeHLayout->addWidget(tribute);
        tributeHLayout->addWidget(new QLabel("Threshold:", this));
        tributeHLayout->addWidget(tributeThreshold);
        tributeHLayout->addWidget(new QLabel("Fleet:", this));
        tributeHLayout->addWidget(tributeFleetName);
        tributeHLayout->addWidget(new QLabel("Quantity:", this));
        tributeHLayout->addWidget(tributeFleetQuantity);
        tributeHLayout->addStretch();
    }
    layout->addWidget(tributeBox, row++, 0, 1, 2);

    setLayout(layout);
}



// Initialize a blank view, or load the existing planet definition for editing.
void PlanetView::SetPlanet(StellarObject *object, const System *system)
{
    this->object = object;
    this->system = system;

    auto it = mapData.Planets().end();
    if(object && !object->GetPlanet().isEmpty())
        it = mapData.Planets().find(object->GetPlanet());

    if(it == mapData.Planets().end())
    {
        // Remove the text from all items in the view.
        for(QLineEdit *textbox : this->findChildren<QLineEdit *>())
            textbox->clear();
        for(QPlainTextEdit *textbox : this->findChildren<QPlainTextEdit *>())
            textbox->clear();

        // Set sane initial values.
        landscape->SetPlanet(nullptr);
        government->setPlaceholderText(system ? system->Government() : QString());
    }
    else
    {
        Planet &planet = it->second;
        name->setText(planet.Name());
        if(planet.GetGovernment().isEmpty())
            government->setPlaceholderText(system ? system->Government() : QString());
        else
            government->setText(planet.GetGovernment());
        attributes->setText(ToString(planet.Attributes()));
        landscape->SetPlanet(&planet);

        disconnect(description, SIGNAL(textChanged()), this, SLOT(DescriptionChanged()));
        description->setPlainText(planet.Description());
        connect(description, SIGNAL(textChanged()), this, SLOT(DescriptionChanged()));

        disconnect(spaceport, SIGNAL(textChanged()), this, SLOT(SpaceportDescriptionChanged()));
        spaceport->setPlainText(planet.SpaceportDescription());
        connect(spaceport, SIGNAL(textChanged()), this, SLOT(SpaceportDescriptionChanged()));

        shipyard->setText(ToString(planet.Shipyard()));
        outfitter->setText(ToString(planet.Outfitter()));

        reputation->setText(std::isnan(planet.RequiredReputation()) ?
            QString() : QString::number(planet.RequiredReputation()));
        bribe->setText(std::isnan(planet.Bribe()) ?
            QString() : QString::number(planet.Bribe()));
        security->setText(std::isnan(planet.Security()) ?
            QString() : QString::number(planet.Security()));

        tribute->setText(std::isnan(planet.Tribute()) ?
            QString() : QString::number(planet.Tribute()));
        tributeThreshold->setText(std::isnan(planet.TributeThreshold()) ?
            QString() : QString::number(planet.TributeThreshold()));
        tributeFleetName->setText(planet.TributeFleetName());
        tributeFleetQuantity->setText(std::isnan(planet.TributeFleetQuantity()) ?
            QString() : QString::number(planet.TributeFleetQuantity()));

    }
}



void PlanetView::Reinitialize()
{
    SetPlanet(nullptr);
    landscape->Reinitialize();
}



// Update the name of the current StellarObject. If previously empty, this will create a planet.
void PlanetView::NameChanged()
{
    if(!object || object->GetPlanet() == name->text() || name->text().isEmpty())
    if(!object || object->GetPlanet() == name->text())
        return;

    // Allow naming a planet after a system, but prompt for confirmation.
    if(mapData.Systems().count(name->text()))
    {
        QString message = "A system named \"" + name->text() + "\" already exists.\n";
        message += "Planets and systems can share the same name, but use of the name in mission definitions will be ambiguous.\n";
        message += "\nDo you really want to make a planet that shares a name with a system?";
        name->blockSignals(true);
        QMessageBox::StandardButton button = QMessageBox::question(this, "Duplicate name", message);
        name->blockSignals(false);
        if(button != QMessageBox::Yes)
        {
            name->setText(object->GetPlanet());
            update();
            return;
        }
    }

    // If this planet is referred to from more than one StellarObject, prompt to determine the desired
    // outcome. The user may want to rename them all, or separate the selected object from the others.
    auto oldPlanet = object->GetPlanet().isEmpty() ?
                mapData.Planets().end() : mapData.Planets().find(object->GetPlanet());
    bool relink = (oldPlanet != mapData.Planets().end() && (oldPlanet->second.IsWormhole()
            || system->PlanetCount(object->GetPlanet()) > 1));
    if(relink)
    {
        QString message = QString();
        if(oldPlanet->second.IsWormhole() && !name->text().isEmpty())
        {
            message += "This planet is part of a wormhole. Would you like to also update the other endpoints?\n";
            message += "\nYes: preserve the wormhole and its links.";
            message += "\nNo: break the link between this object and the rest of the wormhole.";
        }
        else
        {
            // This is a multi-object planet in the same system, e.g. a ringworld.
            message += "This stellar object is part of a multi-object \"planet\". Would you like to change all objects?\n";
            message += "\nYes: rename all objects of the planet (keeping it intact).";
            message += "\nNo: make \"" + name->text() + "\" this object's planet instead of \"" + object->GetPlanet() + ".\"";
        }
        name->blockSignals(true);
        QMessageBox::StandardButton button = QMessageBox::question(this, "Update all stellar objects?", message,
                (QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No), QMessageBox::Cancel);
        name->blockSignals(false);

        // If the user did not choose "Yes" or "No", they closed or canceled the dialog.
        relink = (button == QMessageBox::No);
        if(!relink && button != QMessageBox::Yes)
        {
            name->setText(object->GetPlanet());
            update();
            return;
        }
    }

    // If the new name is an existing planet, special handling may be required.
    auto newPlanet = relink ? mapData.Planets().end() : mapData.Planets().find(name->text());

    // Relinking the planet of an object does not overwrite any existing planet, or erase the old planet.
    if(relink)
        mapData.RelinkObject(object, system, name->text());
    // When the input name matches an existing planet, prompt for confirmation
    // to replace this object's planet with the new one.
    else if(newPlanet != mapData.Planets().end())
    {
        QString title = QString();
        QString message = "\"" + name->text() + "\" is an existing ";
        if(newPlanet->second.IsInSystem(system))
        {
            // Add to / create a "ringworld" planet.
            message +=  "planet in this system. Adding another stellar object to it will ";
            message += "allow the player to land on either to reach the same destination.";
            message += "\nDo you really want to create this kind of planet?";

            title = "Create multi-object planet?";
        }
        else
        {
            // Add to / create a wormhole planet.
            message += newPlanet->second.IsWormhole() ? "wormhole." : "planet in another system. ";
            message += "Adding another instance will create a wormhole link between this system and its other system";
            if(newPlanet->second.IsWormhole())
            {
                message += "s:\n";
                for(const System *system : newPlanet->second.WormholeSystems())
                        message += "\t" + system->Name() + "\n";
            }
            else
                message += ", \"" + newPlanet->second.GetSystem()->Name() + ".\"\n";
            message += "\nDo you really want to create this link?";

            title = "Create wormhole link?";
        }
        name->blockSignals(true);
        QMessageBox::StandardButton button = QMessageBox::question(this, title, message);
        name->blockSignals(false);

        if(button == QMessageBox::Yes)
            mapData.LinkToPlanet(object, system, name->text());
        else
        {
            // Abort the name change.
            name->setText(object->GetPlanet());
            update();
            return;
        }
    }
    // Otherwise, move the existing Planet data from the old name to the new name
    // and update the object's referred planet.
    else
        mapData.RenamePlanet(object, name->text());

    // Update objects that have pointers to this planet.
    Planet &planet = mapData.Planets()[name->text()];
    landscape->SetPlanet(&planet);

    // Ensure this planet knows where it is in the galaxy.
    planet.AddSystem(system);

    mapData.SetChanged();
}



// Change this planet's government. The default government is that of the system.
void PlanetView::GovernmentChanged()
{
    if(!object || object->GetPlanet().isEmpty())
        return;

    Planet &planet = mapData.Planets()[object->GetPlanet()];
    if(planet.GetGovernment() == government->text())
        return;

    // Update the planet's government with the new value.
    planet.SetGovernment(government->text());

    // If the text was deleted from the widget, display the system government
    if(government->text().isEmpty())
        government->setPlaceholderText(planet.GetSystem() ?
                planet.GetSystem()->Government() : QString());

    mapData.SetChanged();
}



void PlanetView::AttributesChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        vector<QString> list = ToList(attributes->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Attributes() != list)
        {
            planet.Attributes() = list;
            mapData.SetChanged();
        }
    }
}



void PlanetView::DescriptionChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        QString newDescription = description->toPlainText();
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Description() != newDescription)
        {
            planet.SetDescription(newDescription);
            mapData.SetChanged();
        }
    }
}



void PlanetView::SpaceportDescriptionChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        QString newDescription = spaceport->toPlainText();
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.SpaceportDescription() != newDescription)
        {
            planet.SetSpaceportDescription(newDescription);
            mapData.SetChanged();
        }
    }
}



void PlanetView::ShipyardChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        vector<QString> list = ToList(shipyard->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Shipyard() != list)
        {
            planet.Shipyard() = list;
            mapData.SetChanged();
        }
    }
}



void PlanetView::OutfitterChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        vector<QString> list = ToList(outfitter->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Outfitter() != list)
        {
            planet.Outfitter() = list;
            mapData.SetChanged();
        }
    }
}



void PlanetView::ReputationChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(reputation->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.RequiredReputation() != value || std::isnan(planet.RequiredReputation()) != std::isnan(value))
        {
            planet.SetRequiredReputation(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::BribeChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(bribe->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Bribe() != value || std::isnan(planet.Bribe()) != std::isnan(value))
        {
            planet.SetBribe(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::SecurityChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(security->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Security() != value || std::isnan(planet.Security()) != std::isnan(value))
        {
            planet.SetSecurity(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::TributeChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(tribute->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.Tribute() != value || std::isnan(planet.Tribute()) != std::isnan(value))
        {
            planet.SetTribute(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::TributeThresholdChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(tributeThreshold->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.TributeThreshold() != value || std::isnan(planet.TributeThreshold()) != std::isnan(value))
        {
            planet.SetTributeThreshold(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::TributeFleetQuantityChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        double value = GetOptionalValue(tributeFleetQuantity->text());
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.TributeFleetQuantity() != value || std::isnan(planet.TributeFleetQuantity()) != std::isnan(value))
        {
            planet.SetTributeFleetQuantity(value);
            mapData.SetChanged();
        }
    }
}



void PlanetView::TributeFleetNameChanged()
{
    if(object && !object->GetPlanet().isEmpty())
    {
        QString newFleetName = tributeFleetName->text();
        Planet &planet = mapData.Planets()[object->GetPlanet()];
        if(planet.TributeFleetName() != newFleetName)
        {
            planet.SetTributeFleetName(newFleetName);
            mapData.SetChanged();
        }
    }
}



QString PlanetView::ToString(const vector<QString> &list)
{
    if(list.empty())
        return "";

    auto it = list.begin();
    QString result = *it;

    for(++it; it != list.end(); ++it)
        result += ", " + *it;

    return result;
}



vector<QString> PlanetView::ToList(const QString &str)
{
    vector<QString> result;

    QStringList strings = str.split(",", QString::SkipEmptyParts);
    for(const QString &token : strings)
        result.emplace_back(token.trimmed());

    return result;
}
