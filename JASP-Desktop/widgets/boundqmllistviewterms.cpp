//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "boundqmllistviewterms.h"
#include "analysis/options/optionvariable.h"
#include "analysis/analysisqmlform.h"
#include "listmodeltermsavailable.h"
#include "analysis/options/optionstable.h"

#include <QQmlProperty>
#include <QTimer>

using namespace std;

BoundQMLListViewTerms::BoundQMLListViewTerms(QQuickItem* item, AnalysisQMLForm* form) : QMLItem(item, form), BoundQMLListViewDraggable(item, form)
{
	_optionsTable = NULL;
	_optionVariables = NULL;
	_singleItem = QQmlProperty(_item, "singleItem").read().toBool();
	_variablesModel = new ListModelTermsAssigned(this, _singleItem);
}

void BoundQMLListViewTerms::bindTo(Option *option)
{	
	if (_hasExtraControlColumns)
	{
		_optionsTable = dynamic_cast<OptionsTable *>(option);
		if (!_optionsTable)
		{
			qDebug() << "Options for list view " << name() << " is not of type Table!";
			return;
		}
		Options* templote = new Options();
		templote->add("variable", new OptionVariable());
		addExtraOptions(templote);
		_optionsTable->setTemplate(templote);
		
		Terms terms;
		std::vector<Options*> optionsList = _optionsTable->value();
		for (Options* options : optionsList)
		{
			OptionVariable* variableOption = dynamic_cast<OptionVariable*>(options->get(0));
			if (variableOption)
				terms.add(variableOption->variable());
		}
		
		// This will draw the table, and call addRowWithControlsHandler that will make the right Bound objects
		_variablesModel->initTerms(terms);
		QTimer::singleShot(0, this, SLOT(_bindExtraControls()));
	}
	else
	{
		_optionVariables = dynamic_cast<OptionVariables *>(option);
		if (_optionVariables)
			_variablesModel->initTerms(_optionVariables->value());
		else
			qDebug() << "Options for list view " << name() << " is not of type Variables!";
	}
}

void BoundQMLListViewTerms::unbind()
{
}

Option* BoundQMLListViewTerms::createOption()
{
	Option* result;
	if (_hasExtraControlColumns)
	{
		Options* options = new Options();
		addExtraOptions(options);
		result = new OptionsTable(options);
	}
	else
		result = _singleItem ? new OptionVariable() : new OptionVariables();
	
	return result;
}

void BoundQMLListViewTerms::modelChangedHandler()
{
	if (_hasExtraControlColumns)
		_connectControlOptions();
	else
		_optionVariables->setValue(_variablesModel->terms().asVectorOfVectors());
}

void BoundQMLListViewTerms::_bindExtraControls()
{
	std::vector<Options*> optionsList = _optionsTable->value();
	for (Options* options : optionsList)
	{
		OptionVariable* variableOption = dynamic_cast<OptionVariable*>(options->get(0));
		if (variableOption)
		{
			QString variable = QString::fromStdString(variableOption->variable());
			if (!_rowsWithControls.contains(variable))
			{
				qDebug() << "Could not find " << variable << " in rows!!!";
				continue;
			}
			const QMap<QString, BoundQMLItem*>& row = _rowsWithControls[variable];
			QMapIterator<QString, BoundQMLItem*> it(row);
			while (it.hasNext())
			{
				it.next();
				std::string name = it.key().toStdString();
				BoundQMLItem* boundItem = it.value();
				Option* option = options->get(name);
				if (!option)
				{
					qDebug() << "Cannot find option " << QString::fromStdString(name);
					continue;
				}
				boundItem->bindTo(option);
			}
		}
	}
}

void BoundQMLListViewTerms::_connectControlOptions()
{
	const Terms& terms = _variablesModel->terms();
	if ((int)(terms.size()) != _rowsWithControls.size())
		qDebug() << "Number of terms " << terms.size() << " is not the same as the number of rows " << _rowsWithControls.size();
	
	std::vector<Options*> oldOptionsList = _optionsTable->value();
	QMap<std::string, Options*> oldOptionsMap;
	for (Options* options : oldOptionsList)
	{
		OptionVariable* variableOption = dynamic_cast<OptionVariable*>(options->get(0));
		if (variableOption)
			oldOptionsMap[variableOption->variable()] = options;
	}
	
	std::vector<Options*> newOptionsList;
	
	for (const Term& term : terms)
	{
		QString termQStr = term.asQString();
		std::string termStr = termQStr.toStdString();
		if (!_rowsWithControls.contains(termQStr))
		{
			qDebug() << "Could not find " << termQStr << " in rows!!!";
			continue;
		}
		
		if (oldOptionsMap.contains(termStr))
		{
			newOptionsList.push_back(oldOptionsMap[termStr]);
		}
		else
		{
			Options* options = new Options();
			OptionVariable* optionVariable = new OptionVariable();
			optionVariable->setValue(termStr);
			options->add("variable", optionVariable);
			
			QMap<QString, BoundQMLItem* > row = _rowsWithControls[termQStr];
			QMapIterator<QString, BoundQMLItem*> it(row);
			while (it.hasNext())
			{
				it.next();
				std::string name = it.key().toStdString();
				BoundQMLItem* boundItem = it.value();
				Option* option = boundItem->createOption();
				options->add(name, option);				
				boundItem->bindTo(option);
			}
			newOptionsList.push_back(options);
		}
	}
	
	_optionsTable->connectOptions(newOptionsList);
}
