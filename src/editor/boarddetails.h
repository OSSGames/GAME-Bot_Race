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

#ifndef BOARDDETAILS_H
#define BOARDDETAILS_H

#include <QDialog>

namespace Ui {
class BoardDetails;
}

namespace BotRace {
namespace Editor {

/**
 * @brief Small Dialog box that lets the user change the details for a board or scenario
 *
 * Details are the additional information such as Name, Author or Description
 */
class BoardDetails : public QDialog {
    Q_OBJECT

public:
    explicit BoardDetails( QWidget *parent = 0 );
    virtual ~BoardDetails();

    /**
     * @brief Defines if the size of the board / scenario can be changed
     *
     * This is aonly active when a new board is created, otherwise the size box is read only
     * @param sizeChangePossible @c true if the size can be changed, @c fale otherwise
     */
    void allowSizeChange( bool sizeChangePossible );

    /**
     * @brief Sets the name of the board/scenario
     * @param name the name
     */
    void setName( const QString &name );

    /**
     * @brief returns the chosen name for the scenario/board
     * @return the name of the scenario/board
     */
    QString getName() const;

    /**
     * @brief Sets the email of the board/scenario author
     * @param email the email of the author
     */
    void setEmail( const QString &email );

    /**
     * @brief Returns the author email
     * @return email of the author
     */
    QString getEmail() const;

    /**
     * @brief Sets a longer description for the board/scenario
     * @param dsc the description
     */
    void setDescription( const QString &dsc );

    /**
     * @brief Returns the longer description
     * @return the description of the board/scenario
     */
    QString getDescription() const;

    /**
     * @brief Sets the name of the board/scenario author
     * @param name the name of the author
     */
    void setAuthor( const QString &name );

    /**
     * @brief Return the name of the board/scenario author
     * @return the board/Scenario author
     */
    QString getAuthor() const;

    /**
     * @brief Sets the size of the board/scenario.
     *
     * This is for the read-only box only. The size can only be changed for a new board.
     * The size of a scenario is determined automatically from the size of each individual board.
     *
     * @param size the size
     */
    void setSize( QSize size );

    /**
     * @brief Returns the user selected size for a new board
     * @return the size of the new board
     */
    QSize getSize() const;

    void setStartOrientation(int startOrientation) const;
    int getStartOrientation() const;

private:
    Ui::BoardDetails *ui; /**< pointer to the ui designer file */
};

}
}

#endif // BOARDDETAILS_H
