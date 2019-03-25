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

#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <QWidget>

namespace Ui {
class GeneralSettings;
}

namespace BotRace {
namespace Core {

/**
 * @brief Widget class to allow general changes
 *
 * Language, gui size, animation speed and such things
*/
class GeneralSettings : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param parent
    */
    explicit GeneralSettings( QWidget *parent = 0 );

    /**
     * @brief Destroctor
    */
    ~GeneralSettings();

public slots:
    /**
     * @brief Called from ConfigDialog to save the made changes
    */
    void applyChanges();

    /**
     * @brief Called from ConfigDialog to revert the changes
    */
    void cancelChanges();

    /**
     * @brief Called when one of the settings is changed to set an internal value indicating this
    */
    void settingsChanged();

private:
    /**
     * @brief Helper function to load the set of available languages
    */
    void loadSettings();

    /**
     * @brief Sets all elements to the state currently in the QSettings file
    */
    void setSavedSettings();

    Ui::GeneralSettings *ui; /**< Pointer to the ui form */
};

}
}

#endif // GENERALSETTINGS_H
