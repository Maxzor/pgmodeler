/*
# PostgreSQL Database Modeler (pgModeler)
#
# Copyright 2006-2019 - Raphael Araújo e Silva <raphael@pgmodeler.io>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/

/**
\ingroup libpgmodeler_ui
\class LayersWidget
\brief Implements the widget that allows the user to handle scene layers
*/

#ifndef LAYERS_WIDGET_H
#define LAYERS_WIDGET_H

#include <QWidget>
#include "ui_layerswidget.h"
#include "modelwidget.h"

class LayersWidget : public QWidget, Ui::LayersWidget {
	private:
		Q_OBJECT

		//! \brief Model in which the layer widget will operate on
		ModelWidget *model;

		/*! \brief The current selected item in the layers list. We need to store it in a separated attribute
		 * for renaming purposes */
		QTreeWidgetItem *curr_item;

		//! \brief Stores the current's item text (layer name) to revert the renaming if the user aborts it
		QString curr_text;

		//! \brief Stores the current's item row
		int curr_row;

		//! \brief Stores the current's item dimension
		int curr_dim;

		//! \brief Configures the layers listing
		void updateLayers(void);

		bool eventFilter(QObject *watched, QEvent *event);

	public:
		explicit LayersWidget(QWidget *parent = nullptr);

		//! \brief Defines the model in which the widget will work
		void setModel(ModelWidget *model);

	private slots:
		//! \brief Add a new item (layer) to the listing. If the provided name is empty a default name is assigned
		QTreeWidgetItem *addLayer(const QString &name = QString());

		//! \brief Triggers the renaming operation over a item
		void startLayerRenaming(QTreeWidgetItem *item);

		//! \brief Finishes the renaming operation over a item
		void finishLayerRenaming(void);

		//! \brief Updates the active layeres on the scene causing a redraw of the items
		void updateActiveLayers(void);

		//! \brief Remove a layer from the listing. If 'clear' is true them all layers (except the default) are removed
		void removeLayer(bool clear = false);

		void addDimension(bool begin, int d_idx, QStringList dim);

		void removeDimension(void);

		//! \brief Enables the control buttons according to the selection on the list
		void enableButtons(void);

		void updateSelection();

	public slots:
		void setVisible(bool value);

	signals:
		//! \brief Signal emitted whenever the widget changes its visibility
		void s_visibilityChanged(bool);

		//! \brief Signal emitted whenever the current active layers change
		void s_activeLayersChanged(void);
};

#endif
