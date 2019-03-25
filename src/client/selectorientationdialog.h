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

#ifndef SELECTORIENTATIONDIALOG_H
#define SELECTORIENTATIONDIALOG_H

#include <QDialog>
#include "engine/robot.h"

namespace Ui {
class SelectOrientationDialog;
}

namespace BotRace {
namespace Client {

/**
 * @brief This dialog let the user decide in which direction a robot should look to when it is resurrected
 *
 * @see StateCleanUp::resurrectPlayers
 * @see BoardManager::allowedStartingOrientations
*/
class SelectOrientationDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Creates the modal dialog for the orientatio nselection
     *
     * @param parent parent widget
    */
    explicit SelectOrientationDialog( QWidget *parent = 0 );

    /**
     * @brief destructor
    */
    virtual ~SelectOrientationDialog();

    /**
     * @brief Enables all Orientations which are possible
     *
     * It is not possible to select a orientation in which the robot would look
     * directly to another robot.
     *
     * @param orientationList list of available orientations
     *
     * @see BoardManager::allowedStartingOrientations
    */
    void setAvailableOrientations( QList<Core::Orientation> orientationList );

    /**
     * @brief Returns the user selection
    */
    Core::Orientation getSelectedOrientation();

private:
    Ui::SelectOrientationDialog *ui; /**< Pointer to the ui form */
};

}
}

#endif // SELECTORIENTATIONDIALOG_H
