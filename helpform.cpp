#include "helpform.h"
#include "ui_helpform.h"
#include <QFile>
#include <QTextStream>

HelpForm::HelpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpForm)
{
    ui->setupUi(this);

    ui->HelpWebView->setHtml("<?xml version='1.0' encoding='UTF-8'?>" \
                             "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN' 'http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd'>" \
                             "<html xmlns='http://www.w3.org/1999/xhtml'><!--This file was converted to xhtml by OpenOffice.org - see http://xml.openoffice.org/odf2xhtml for more info.--><head profile='http://dublincore.org/documents/dcmi-terms/'><meta http-equiv='Content-Type' content='application/xhtml+xml; charset=utf-8'/><title xml:lang='en-US'>- no title specified</title>" \
                             "<meta name='DCTERMS.title' content='' xml:lang='en-US'/><meta name='DCTERMS.language' content='en-US' scheme='DCTERMS.RFC4646'/><meta name='DCTERMS.source' content='http://xml.openoffice.org/odf2xhtml'/><meta name='DCTERMS.creator' content='jan '/><meta name='DCTERMS.issued' content='2014-08-08T14:28:12' scheme='DCTERMS.W3CDTF'/><meta name='DCTERMS.contributor' content='jan '/><meta name='DCTERMS.modified' content='2014-08-12T07:46:34' scheme='DCTERMS.W3CDTF'/><meta name='DCTERMS.provenance' content='' xml:lang='en-US'/><meta name='DCTERMS.subject' content=',' xml:lang='en-US'/><link rel='schema.DC' href='http://purl.org/dc/elements/1.1/' hreflang='en'/><link rel='schema.DCTERMS' href='http://purl.org/dc/terms/' hreflang='en'/><link rel='schema.DCTYPE' href='http://purl.org/dc/dcmitype/' hreflang='en'/><link rel='schema.DCAM' href='http://purl.org/dc/dcam/' hreflang='en'/><style type='text/css'>" \
                                 "@page {  }" \
                                 "table { border-collapse:collapse; border-spacing:0; empty-cells:show }" \
                                 "td, th { vertical-align:top; font-size:12pt;}" \
                                 "h1, h2, h3, h4, h5, h6 { clear:both }" \
                                 "ol, ul { margin:0; padding:0;}" \
                                 "li { list-style: none; margin:0; padding:0;}" \
                                 "<!-- 'li span.odfLiEnd' - IE 7 issue-->" \
                                 "li span. { clear: both; line-height:0; width:0; height:0; margin:0; padding:0; }" \
                                 "span.footnodeNumber { padding-right:1em; }" \
                                 "span.annotation_style_by_filter { font-size:95%; font-family:Arial; background-color:#fff000;  margin:0; border:0; padding:0;  }" \
                                 "* { margin:0;}" \
                                 ".P1 { font-size:12pt; font-family:Liberation Serif; writing-mode:page; font-weight:normal; }" \
                                 ".P2 { font-size:22pt; font-family:Liberation Serif; writing-mode:page; text-align:center ! important; font-weight:bold; }" \
                                 ".P3 { font-size:12pt; font-family:Liberation Serif; writing-mode:page; color:#c5000b; }" \
                                 ".P4 { font-size:12pt; font-family:Liberation Serif; writing-mode:page; color:#008000; }" \
                                 ".P5 { font-size:22pt; font-family:Liberation Serif; writing-mode:page; }" \
                                 ".Standard { font-size:12pt; font-family:Liberation Serif; writing-mode:page; }" \
                                 ".T1 { font-weight:bold; }" \
                                 ".T2 { font-style:italic; font-weight:bold; }" \
                                 ".T3 { font-weight:normal; }" \
                                 "<!-- ODF styles with no properties representable as CSS -->" \
                                 ".Numbering_20_Symbols { }" \
                                 "</style></head><body dir='ltr' style='max-width:8.5in;margin:0.7874in; margin-top:0.7874in; margin-bottom:0.7874in; margin-left:0.7874in; margin-right:0.7874in; writing-mode:lr-tb; '><p class='P2'>Aide</p><p class='Standard'> </p><ol><li><p class='P5' style='margin-left:0cm;'><span class='Numbering_20_Symbols' style='display:block;float:left;min-width:0cm'>1.</span>Ajouter une variété d'ananas ou de banane.<span class='odfLiEnd'/> </p></li></ol><p class='Standard'> </p><p class='Standard'>Lors de l'ajout d'une variété dans l'onglet <span class='T1'>“Variétés”, “Ajouter une variété”</span> Les champs <span class='T1'>T°C base 1</span> et <span class='T1'>T°C floraison</span> servent à déterminer la date de floraison à partir de la date de traitement d'induction florale. Ces champs ne concernent uniquement les variétés d'ananas et doivent par conséquent rester vide pour les variétés de bananes.</p><p class='Standard'>La date de floraison est la date à laquelle la somme des<span class='T1'> températures journalières</span> moins la <span class='T1'>température journalière de base</span> est égal à la <span class='T1'>température de floraison</span>.</p>" \
                                "<p class='Standard'><span class='T1'>T°C base 2</span> et <span class='T1'>T°C récolte</span> quand à eux servent à déterminer la date récolte en fonction de la date de floraison.</p><p class='Standard'> </p><ol><li><p class='P5' style='margin-left:0cm;'><span class='Numbering_20_Symbols' style='display:block;float:left;min-width:0cm'>2.</span>importer des données météorologiques.<span class='odfLiEnd'/> </p></li></ol>" \
                                "<p class='P4'> </p><p class='Standard'>Le logiciel supporte l'import des données météo au format <span class='T1'>CSV.</span></p><p class='P1'>Afin que le fichier <span class='T1'>CSV</span> soit importable il est nécéssaire de le formatter de manière spécifique sous un tableur comme <span class='T2'>Excel</span> ou <span class='T2'>Libre Office</span>.</p><p class='Standard'>Le tableau doit contenir une colonne date et à droite de celle-ci doit se trouver une colonne température.</p><p class='Standard'>Les dates doivent être sous le format <span class='T1'>yyyy-MM-dd.</span></p><p class='Standard'>Ci-dessous des exemples de dates valides et invalides:</p><p class='P4'>2009-12-04 Valide.</p><p class='P4'>2009-12-04 23:06:12 Valide.</p><p class='P3'>2009-12-4 Invalide</p><p class='P4'>2009-02-04 Valide.</p><p class='P3'>2009-2-04 Invalide</p><p class='Standard'> </p><p class='Standard'>Les températures doivent se trouver sous le format 0.000 .</p><p class='Standard'>Exemples de températures valides et invalides:</p><p class='Standard'> </p><p class='P4'>9.890 Valide.</p><p class='P3'>9,890 Invalide.</p><p class='P4'>19.890 Valide.</p><p class='P3'>19.89 Invalide.</p><p class='P3'>19 Invalide</p><p class='Standard'> </p><p class='Standard'>/!\ Le logiciel ne prend pas en compte les températures négatives.</p><p class='Standard'> </p>" \
                                "<ol><li><p class='P5' style='margin-left:0cm;'><span class='Numbering_20_Symbols' style='display:block;float:left;min-width:0cm'>3.</span>Exporter des données météorologiques.<span class='odfLiEnd'/> </p></li></ol><p class='Standard'> </p><p class='Standard'>Il est possible d'exporter les données météorologiques dans l'onglet <span class='T1'>“Sites”, “Editer un site”</span> et en sélectionnant un site ou une date. En cliquant sur “Exporter” après avoir sélectionné un site l'on exporte toutes les données météo du site. Si l'on a sélectionné une année l'on exporte uniquement les données météo de l'année sélectionnée.</p><p class='Standard'>Les données météo sont exportées sous le format <span class='T1'>CSV</span>.</p><p class='Standard'>Le fichier exporté contient 4 colonnes :</p><p class='Standard'>ID, date (jour), température moyenne du jour, température minimale du jour, température maximale du jour.</p><p class='Standard'> </p><p class='Standard'> </p><p class='Standard'> </p><ol><li><p class='P5' style='margin-left:0cm;'><span class='Numbering_20_Symbols' style='display:block;float:left;min-width:0cm'>4.</span>Calculer la date de floraison et de récolte.<span class='odfLiEnd'/> </p></li></ol><p class='Standard'> </p><p class='Standard'>Avant de calculer  la date de floraison et de récolte il faut s'assurer ques les données météo du sites sont à jour afin d'avoir le résultat le plus précis possible.</p>" \
                                "<p class='Standard'>Le calcul de la date de floraison et de récolte se fait dans l'onglet <span class='T1'>“Prévisions” </span><span class='T3'>dans lequel il faut renseigner la variété, une date puis sélectionner à quel événement cette date correspond (floraison ou TIF). Enfin sélectionner le modèle de prévision qui peut être une année précédante ou la moyenne de toutes les années présentes dans la base de données.</span></p><p class='Standard'><span class='T3'>Après avoir cliqué sur le boutton </span><span class='T1'>“Calculer la date de récolte”</span><span class='T3'> les dates prévues s'affichent alors sur la droite.</span></p><p class='Standard'><span class='T3'/></p><p class='Standard'><span class='T3'>Si un message du type “Valeur manquante pour la date du ….” s'affiche cela signifie qu'il manque des données météo pour une date dans le modèle sélectionné. Il faut à ce moment soit changer de modèle ou alors importer des données météo supplémentaires.</span></p></body></html>");

}

HelpForm::~HelpForm()
{
    delete ui;
}
