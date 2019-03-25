/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCENARIOSELECTIONDIALOG_H
#define SCENARIOSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ScenarioSelectionDialog;
}

class QTreeWidgetItem;

namespace BotRace {
namespace Client {

/**
 * @brief Used in the NewGameDialog to select the scenario which should be played
 *
 * Let the user decide which scenario should be played and shows some information
 * about the selected scenario.
 *
 * Finds scenarios in in 3 different locations:
 * @li $InstallDir/boards
 * @li $currentDir/boards
 * @li $Home/.botrace/boards
 *
*/
class ScenarioSelectionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param parent the parent widget
    */
    explicit ScenarioSelectionDialog( QWidget *parent = 0 );

    /**
     * @brief Destructor
     *
    */
    virtual ~ScenarioSelectionDialog();

    /**
     * @brief Returns the selected scenario
     *
    */
    QString getSelection();

private slots:
    /**
     * @brief Updates the scenario details when a new one is selected in the list
     *
     * @param currentText
    */
    void updateDetails();

private:
    /**
     * @brief Helper function to find and list all available board scenarios
     *
    */
    void fillScenarioList();

    /**
     * @brief addScenario Helper function to create the QTreeView items
     * @param scenarioName the name of the scenario (filename without .scenario)
     * @param path the path of the scenario
     */
    void addScenario(const QString &scenarioName, const QString &path);


    Ui::ScenarioSelectionDialog *ui; /**< Pointer to the ui form */

    QTreeWidgetItem *m_smallScenarios;
    QTreeWidgetItem *m_mediumScenarios;
    QTreeWidgetItem *m_largeScenarios;
};

}
}

#endif // SCENARIOSELECTIONDIALOG_H
