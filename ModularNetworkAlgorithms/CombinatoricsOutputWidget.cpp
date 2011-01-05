#include "CombinatoricsOutputWidget.h"

using namespace Tinkercell;

ModuleCombinatoricsOutputWidget::ModuleCombinatoricsOutputWidget(
	const NumericalDataTable& population, 
	const NumericalDataTable& modules, 
	const QList<qreal>& scores) : 
	QDialog()
{
	setAttribute(Qt::WA_DeleteOnClose);
	
}

