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

#include "metadatahandlingform.h"
#include "pgmodeleruins.h"
#include <QFileDialog>

MetadataHandlingForm::MetadataHandlingForm(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	delete horizontalLayout_3->takeAt(horizontalLayout_3->indexOf(label_5));
	delete label_5;
	f_sel_wgt=new FileSelectorWidget(tab);
	// does not "work" here?
	//delete horizontalLayout_3->replaceWidget(label_5, f_sel_wgt,Qt::FindChildrenRecursively);
	horizontalLayout_3->addWidget(f_sel_wgt);



	root_item=nullptr;
	model_wgt=nullptr;
	settings_tbw->setTabEnabled(1, false);
	apply_btn->setEnabled(false);

	db_metadata_ht=new HintTextWidget(db_metadata_hint, this);
	db_metadata_ht->setText(db_metadata_chk->statusTip());

	objs_positioning_ht=new HintTextWidget(objs_positioning_hint, this);
	objs_positioning_ht->setText(objs_positioning_chk->statusTip());

	objs_protection_ht=new HintTextWidget(objs_protection_hint, this);
	objs_protection_ht->setText(objs_protection_chk->statusTip());

	objs_sql_disabled_ht=new HintTextWidget(objs_sql_disabled_hint, this);
	objs_sql_disabled_ht->setText(objs_sql_disabled_chk->statusTip());

	objs_fadedout_ht=new HintTextWidget(objs_fadedout_hint, this);
	objs_fadedout_ht->setText(objs_fadedout_chk->statusTip());

	objs_collapse_mode_ht=new HintTextWidget(objs_collapse_mode_hint, this);
	objs_collapse_mode_ht->setText(objs_collapse_mode_chk->statusTip());

	custom_sql_ht=new HintTextWidget(custom_sql_hint, this);
	custom_sql_ht->setText(custom_sql_chk->statusTip());

	textbox_objs_ht=new HintTextWidget(textbox_objs_hint, this);
	textbox_objs_ht->setText(textbox_objs_chk->statusTip());

	tag_objs_ht=new HintTextWidget(tag_objs_hint, this);
	tag_objs_ht->setText(tag_objs_chk->statusTip());

	custom_colors_ht=new HintTextWidget(custom_colors_hint, this);
	custom_colors_ht->setText(custom_colors_chk->statusTip());

	extract_restore_ht=new HintTextWidget(extract_restore_hint, this);
	extract_restore_ht->setText(extract_restore_rb->statusTip());

	extract_only_ht=new HintTextWidget(extract_only_hint, this);
	extract_only_ht->setText(extract_only_rb->statusTip());

	restore_ht=new HintTextWidget(restore_hint, this);
	restore_ht->setText(restore_rb->statusTip());

	generic_sql_objs_ht=new HintTextWidget(generic_sql_objs_hint, this);
	generic_sql_objs_ht->setText(generic_sql_objs_chk->statusTip());

	objs_aliases_ht=new HintTextWidget(objs_aliases_hint, this);
	objs_aliases_ht->setText(objs_aliases_chk->statusTip());

	htmlitem_deleg=new HtmlItemDelegate(this);
	output_trw->setItemDelegateForColumn(0, htmlitem_deleg);

	connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	connect(apply_btn, SIGNAL(clicked()), this, SLOT(handleObjectsMetada()));

	connect(extract_from_cmb, &QComboBox::currentTextChanged,
					[&](){ apply_btn->setDisabled(extract_from_cmb->count() == 0); });

	connect(restore_rb, SIGNAL(toggled(bool)), this, SLOT(updateFileSelector()));
	connect(extract_restore_rb, SIGNAL(toggled(bool)), this, SLOT(updateFileSelector()));
	connect(extract_only_rb, SIGNAL(toggled(bool)), this, SLOT(updateFileSelector()));
	connect(extract_from_cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(enableMetadataHandling()));
	connect(f_sel_wgt, SIGNAL(s_fileSelected(QFileInfo*)), this, SLOT(enableMetadataHandling()));
	connect(restore_rb, SIGNAL(toggled(bool)), this, SLOT(enableMetadataHandling()));
	connect(extract_restore_rb, SIGNAL(toggled(bool)), this, SLOT(enableMetadataHandling()));
	connect(extract_only_rb, SIGNAL(toggled(bool)), this, SLOT(enableMetadataHandling()));
	connect(select_all_btn, SIGNAL(clicked(bool)), this, SLOT(selectAllOptions()));
	connect(clear_all_btn, SIGNAL(clicked(bool)), this, SLOT(selectAllOptions()));

	updateFileSelector();
}

void MetadataHandlingForm::enableMetadataHandling()
{
	extract_from_cmb->setVisible(!restore_rb->isChecked());
	extract_from_lbl->setVisible(!restore_rb->isChecked());
	apply_to_lbl->setVisible(!extract_only_rb->isChecked());
	apply_to_edt->setVisible(!extract_only_rb->isChecked());

	apply_btn->setEnabled(model_wgt &&
							(((extract_restore_rb->isChecked() && extract_from_cmb->count() > 0) ||
								(extract_only_rb->isChecked() && extract_from_cmb->count() > 0 && !f_sel_wgt->currentFileString().isEmpty()) ||
								(restore_rb->isChecked() && !f_sel_wgt->currentFileString().isEmpty()))));
}

void MetadataHandlingForm::selectAllOptions()
{
	bool check = sender() == select_all_btn;
	QCheckBox *checkbox = nullptr;

	for(auto &obj : options_grp->children())
	{
		checkbox = dynamic_cast<QCheckBox *>(obj);

		if(checkbox)
			checkbox->setChecked(check);
	}
}

void MetadataHandlingForm::setModelWidget(ModelWidget *model_wgt)
{
	this->model_wgt=model_wgt;

	apply_to_edt->clear();

	if(model_wgt)
	{
		apply_to_edt->setText(QString("%1 (%2)").arg(model_wgt->getDatabaseModel()->getName())
													.arg(model_wgt->getFilename().isEmpty() ? tr("model not saved yet") : model_wgt->getFilename()));
	}
}

void MetadataHandlingForm::setModelWidgets(QList<ModelWidget *> models)
{
	extract_from_cmb->clear();

	for(ModelWidget *model : models)
	{
		extract_from_cmb->addItem(QString("%1 (%2)").arg(model->getDatabaseModel()->getName())
															.arg(model->getFilename().isEmpty() ? tr("model not saved yet") : model->getFilename()),
															QVariant::fromValue<void *>(reinterpret_cast<void *>(model->getDatabaseModel())));
	}
}

void MetadataHandlingForm::handleObjectsMetada()
{
	if(!f_sel_wgt->currentFileString().isEmpty() &&
		 f_sel_wgt->currentFileString() == model_wgt->getFilename())
		throw Exception(tr("The backup file cannot be the same as the input model!"),
										ErrorCode::Custom,	__PRETTY_FUNCTION__,__FILE__,__LINE__);

	QTemporaryFile tmp_file;
	QString metadata_file;
	unsigned options=0;
	DatabaseModel *extract_model=nullptr;

	try
	{
		root_item=nullptr;
		output_trw->clear();
		settings_tbw->setTabEnabled(1, true);
		settings_tbw->setCurrentIndex(1);

		options+=(db_metadata_chk->isChecked() ? DatabaseModel::MetaDbAttributes : 0);
		options+=(custom_colors_chk->isChecked() ? DatabaseModel::MetaObjsCustomColors : 0);
		options+=(custom_sql_chk->isChecked() ? DatabaseModel::MetaObjsCustomSql : 0);
		options+=(objs_positioning_chk->isChecked() ? DatabaseModel::MetaObjsPositioning : 0);
		options+=(objs_protection_chk->isChecked() ? DatabaseModel::MetaObjsProtection : 0);
		options+=(objs_sql_disabled_chk->isChecked() ? DatabaseModel::MetaObjsSqlDisabled : 0);
		options+=(tag_objs_chk->isChecked() ? DatabaseModel::MetaTagObjs : 0);
		options+=(textbox_objs_chk->isChecked() ? DatabaseModel::MetaTextboxObjs : 0);
		options+=(objs_fadedout_chk->isChecked() ? DatabaseModel::MetaObjsFadeOut : 0);
		options+=(objs_collapse_mode_chk->isChecked() ? DatabaseModel::MetaObjsCollapseMode : 0);
		options+=(generic_sql_objs_chk->isChecked() ? DatabaseModel::MetaGenericSqlObjs : 0);
		options+=(objs_aliases_chk->isChecked() ? DatabaseModel::MetaObjsAliases : 0);

		connect(model_wgt->getDatabaseModel(), SIGNAL(s_objectLoaded(int,QString,unsigned)), this, SLOT(updateProgress(int,QString,unsigned)), Qt::UniqueConnection);

		if(extract_restore_rb->isChecked() || extract_only_rb->isChecked())
		{
			extract_model=reinterpret_cast<DatabaseModel *>(extract_from_cmb->currentData(Qt::UserRole).value<void *>());

			if(extract_only_rb->isChecked())
				metadata_file = f_sel_wgt->currentFileString();
			else
			{
				//Configuring the temporary metadata file
				tmp_file.setFileTemplate(GlobalAttributes::getTemporaryDir() +
															 GlobalAttributes::DirSeparator +
															 QString("%1_metadata_XXXXXX.%2").arg(extract_model->getName()).arg(QString("omf")));

				tmp_file.open();
				metadata_file=tmp_file.fileName();
				tmp_file.close();
			}

			connect(extract_model, SIGNAL(s_objectLoaded(int,QString,unsigned)), this, SLOT(updateProgress(int,QString,unsigned)), Qt::UniqueConnection);

			root_item=PgModelerUiNs::createOutputTreeItem(output_trw,
										PgModelerUiNs::formatMessage(tr("Extracting metadata to file `%1'").arg(metadata_file)),
										QPixmap(PgModelerUiNs::getIconPath("msgbox_info")), nullptr);

			extract_model->saveObjectsMetadata(metadata_file, options);

			if(extract_restore_rb->isChecked() && !f_sel_wgt->currentFileString().isEmpty())
			{
				root_item->setExpanded(false);
				root_item=PgModelerUiNs::createOutputTreeItem(output_trw,
											PgModelerUiNs::formatMessage(tr("Saving backup metadata to file `%1'").arg(f_sel_wgt->currentFileString())),
											QPixmap(PgModelerUiNs::getIconPath("msgbox_info")), nullptr);

				model_wgt->getDatabaseModel()->saveObjectsMetadata(f_sel_wgt->currentFileString());
			}
		}
		else
		{
			metadata_file=f_sel_wgt->currentFileString();
		}

		if(root_item)
			root_item->setExpanded(false);

		if(!extract_only_rb->isChecked())
		{
			root_item=PgModelerUiNs::createOutputTreeItem(output_trw,
					PgModelerUiNs::formatMessage(tr("Applying metadata from file `%1'").arg(metadata_file)),
					QPixmap(PgModelerUiNs::getIconPath("msgbox_info")), nullptr);

			model_wgt->setUpdatesEnabled(false);
			model_wgt->getDatabaseModel()->loadObjectsMetadata(metadata_file, options);
			model_wgt->adjustSceneSize();
			model_wgt->restoreLastCanvasPosition();
			model_wgt->setUpdatesEnabled(true);
			model_wgt->setModified(true);
			model_wgt->updateObjectsOpacity();
		}

		disconnect(model_wgt->getDatabaseModel(), nullptr, this, nullptr);

		if(extract_model)
			disconnect(extract_model, nullptr, this, nullptr);

		emit s_metadataHandled();
	}
	catch(Exception &e)
	{
		QPixmap icon=QPixmap(PgModelerUiNs::getIconPath("msgbox_erro"));

		disconnect(model_wgt->getDatabaseModel(), nullptr, this, nullptr);

		if(extract_model)
			disconnect(extract_model, nullptr, this, nullptr);

		PgModelerUiNs::createOutputTreeItem(output_trw,
				PgModelerUiNs::formatMessage(e.getErrorMessage()),
				icon, nullptr);

		ico_lbl->setPixmap(icon);
		progress_lbl->setText(tr("Metadata processing aborted!"));

		throw Exception(e.getErrorMessage(),e.getErrorCode(),__PRETTY_FUNCTION__,__FILE__,__LINE__, &e);
	}
}

void MetadataHandlingForm::showEvent(QShowEvent *)
{
	if(!model_wgt)
	{
		apply_btn->setEnabled(false);
		settings_tbw->setEnabled(false);
	}
}

void MetadataHandlingForm::updateProgress(int progress, QString msg, unsigned int type_id)
{
	ObjectType obj_type=static_cast<ObjectType>(type_id);
	QString fmt_msg=PgModelerUiNs::formatMessage(msg);
	QPixmap icon;

	if(obj_type==ObjectType::BaseObject)
	{
		if(progress==100)
			icon=QPixmap(PgModelerUiNs::getIconPath("msgbox_info"));
		else
			icon=QPixmap(PgModelerUiNs::getIconPath("msgbox_alerta"));
	}
	else
		icon=QPixmap(PgModelerUiNs::getIconPath(obj_type));

	PgModelerUiNs::createOutputTreeItem(output_trw, fmt_msg, icon, root_item);
	progress_lbl->setText(fmt_msg);
	ico_lbl->setPixmap(icon);
	progress_pb->setValue(progress);
}

void MetadataHandlingForm::updateFileSelector(){
	extern FileSelector FSel_SQLOutput;
	auto old_d_sel=f_sel_wgt->file_selector;
	if(extract_restore_rb->isChecked() || extract_only_rb->isChecked())
	{
		f_sel_wgt->file_selector=	new FileSelector{
				QFileDialog::AcceptSave, QFileDialog::AnyFile,
				"Save object-metadata file :","omf", {""},{"Objects metadata file (*.omf);;All files (*.*)"},
				{"","","Set","Cancel"},	{false, false},	false};
		//todo f_sel_wgt.selectFile(f_sel_wgt->currentFileString());
	}
	else
		f_sel_wgt->file_selector=	new FileSelector{
				QFileDialog::AcceptOpen, QFileDialog::ExistingFile,
				"Object-metadata file input :", "omf", {""},{"Objects metadata file (*.omf);;All files (*.*)"},
				{"","","Set","Cancel"},	{false, true},	false};
		//todo f_sel_wgt.setFile(model_wgt->getDatabaseModel()->getName() + QString(".omf"));
	if(old_d_sel!=&FSel_SQLOutput)
		delete old_d_sel;

	//Update the string
	f_sel_wgt->handleTextChanged(f_sel_wgt->currentFileString());
}
