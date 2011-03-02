#include <QVector>
extern "C"
{
	#include "cluster.h"
}

namespace Tinkercell
{
	class ClusterPlot
	{
		public:
			static int* getClusters(int nclusters, const QList<NumericalDataTable*> & tables)
			{
				int nrows = tables.size();
				int ncols = -1;
				
				int m = -1, n = -1;
				
				for (int i=0; i < tables.size(); ++i)
					if (tables[i])
					{ 
						if (m < 0 || m > tables[i]->rows())
							m = tables[i]->rows();
							
						if (n < 0 || n > tables[i]->columns())
							n = tables[i]->columns();
					}
				
				ncols = m*n;
				
				double ** data = new double*[nrows];
				int** mask = new int*[nrows];
				
				for (int i=0; i < nrows; ++i)
				{
					data[i] = new double[ncols];
					mask[i] = new int[ncols];
					
					for (int j=0; j < ncols; ++j)
					{
						data[i][j] = 0.0;
						mask[i][j] = 1;
					}
					
					l = 0;
					
					if (tables[i])
						for (int j=0; j < n; ++j)
							for (int k=0; k < m; ++k, ++l)
							{
								data[i][l] = tables[i]->at(k,j);
							}
				}

				int transpose = 0;
				char dist = 'e';
				char method = 'a';
				int npass = 1;
				int ifound = 0;
				double error;
				double distance;
				double* weight = new double[ncols];
				int* clusterid = new int[nrows];
				
				for (int i = 0; i < ncols; ++i) 
					weight[i] = 1.0;

				kcluster(nclusters,nrows,ncols,data,mask,weight,transpose,npass,method,dist, clusterid, &error, &ifound);
				
				delete weight;
				for (int i=0; i < nrows; ++i)
				{
					delete data[i];
					delete mask[i];
				}
				delete data;
				delete mask;
				
				return clusterid;
			}
	};
}

