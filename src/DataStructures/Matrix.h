#ifndef MATRIX_H_
#define MATRIX_H_

#include "../Toolbox.h"
#include "../Serializable.h"
#include "KTuple.h"

#include <string.h>
#include <stdio.h>
#include <string>
#include <sys/mman.h>
#include <cmath>

#include <type_traits>

#ifdef LINUX
#include <clocale>
#endif
#ifdef OSX
#include <locale.h>
#endif

namespace Lazarus
{

// we are using n x m matrices !

//row major format (AS IT SHOULD BE)
//transforms 2-dim coordinates into 1-dim memory position
//with offset all 2-dim coordinates start at 1
/*
 In case indices starts from 1
 i  : row index
 j  : column index
 ld : size of a row
 */
#define IDX2ARRAYCOORDINATE_RM_OFFSET_DIM2(i,j,lr) ( (((i)-1)*(lr)) + ((j)-1) )

/*
 In case indices starts from 0 (AS IT SHOULD BE!)
 i,j,ld : Same as above
 */
#define IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,lr) ( ((i)*(lr))+(j) )

//column major format (just as CUDA desires)
/*
 In case indices starts from 1
 i  : row index
 j  : column index
 ld : size of a column
 */
#define IDX2ARRAYCOORDINATE_CM_OFFSET_DIM2(i,j,lc) ( (((j)-1)*(lc)) + ((i)-1) )

/*
 In case indices starts from 0
 i,j,ld : Same as above
 */
#define IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,lc) ( ((j)*(lc))+(i) )

//analogously for 3 dims
#define IDX2ARRAYCOORDINATE_RM_OFFSET_DIM3(i,j,k,lr,lc) ( (((k)-1)*(lr*lc)) + (((i)-1)*(lr)) + ((j)-1) )
#define IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,lr,lc) ( (((k) )*(lr*lc)) + (((i) )*(lr)) + ((j) ) )
#define IDX2ARRAYCOORDINATE_CM_OFFSET_DIM3(i,j,k,lr,lc) ( (((k)-1)*(lr*lc)) + (((j)-1)*(lc)) + ((i)-1) )
#define IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,lr,lc) ( (((k) )*(lr*lc)) + (((j) )*(lc)) + ((i) ) )

//******************* generic base class ************************

class Matrix : public Serializable
{
public:

	/*enum MATRIX_DATA_TYPE {MATRIX_DATA_TYPE_DOUBLE, MATRIX_DATA_TYPE_FLOAT,
		MATRIX_DATA_TYPE_LONG_DOUBLE, MATRIX_DATA_TYPE_INT, MATRIX_DATA_TYPE_LONGLONG, MATRIX_DATA_TYPE_ULONGLONG,
		MATRIX_DATA_TYPE_LONG, MATRIX_DATA_TYPE_ULONG,MATRIX_DATA_TYPE_CHAR,
		MATRIX_DATA_TYPE_SHORT,MATRIX_DATA_TYPE_USHORT,
		MATRIX_DATA_TYPE_UINT, MATRIX_DATA_TYPE_UCHAR, MATRIX_DATA_TYPE_ABSTRACT, MATRIX_DATA_TYPE_UNSPECIFIED};*/

	enum MATRIX_DATA_ALIGNMENT {MATRIX_DATA_ALIGNMENT_ROW_MAJOR,MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR};


	Matrix( enum MATRIX_DATA_ALIGNMENT data_alignment = MATRIX_DATA_ALIGNMENT_ROW_MAJOR,
			enum SH_DATA_TYPE data_type = SH_DATA_TYPE_FLOAT )
	{
		this->data_alignment = data_alignment;
		this->data_type = data_type;
	}

	virtual ~Matrix()
	{

	}

	CLASS_RTTI(Lazarus::Matrix)

protected:

    enum MATRIX_DATA_ALIGNMENT data_alignment;
    enum SH_DATA_TYPE data_type;

};


//******************* 2 dims *******************************

template<class T> class Matrix2 : public Matrix
{
public:
		/**
		 * This class is merely a state container in order to use the [][]
		 * operator on a matrix, i.e. mat[] returns a row pointer object X
		 * which has the desired row >index< temporarily saved.
		 * X[] will finally deliver the value in this row,column element.
		 * */
		class Matrix2RowPointer
		{
			friend class Matrix2<T>;
		public:
			T& operator[](unsigned int col)
			{
				if(m_row>=mp_matrix->m_rows)
				{
					printf("ERROR: getData row %d not available\n",m_row);
					return mp_matrix->data[0];
				}
				if(col>=mp_matrix->m_columns)
				{
					printf("ERROR: getData column %d not available\n",col);
					return mp_matrix->data[0];
				}

				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->rows)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(m_row,col,mp_matrix->m_rows)];
				}
				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->columns)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(m_row,col,mp_matrix->m_columns)];
				}

				return mp_matrix->data[0];
			}

			const T& operator[](unsigned int col) const
			{
				if(m_row>=mp_matrix->m_rows)
				{
					printf("ERROR: getData row %d not available\n",m_row);
					return mp_matrix->data[0];
				}
				if(col>=mp_matrix->m_columns)
				{
					printf("ERROR: getData column %d not available\n",col);
					return mp_matrix->data[0];
				}

				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->rows)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(m_row,col,mp_matrix->m_rows)];
				}
				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->columns)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(m_row,col,mp_matrix->m_columns)];
				}

				return mp_matrix->data[0];
			}

		private:
			Matrix2RowPointer(Matrix2<T>* mat, int row)
			{
				mp_matrix = mat;
				m_row = row;
			}
			Matrix2<T>* mp_matrix;//external ref
			unsigned int m_row;

		};

        Matrix2(enum MATRIX_DATA_ALIGNMENT data_alignment = MATRIX_DATA_ALIGNMENT_ROW_MAJOR, bool pinned = false)
		{
			this->m_columns = 0;
			this->m_rows = 0;

			this->data_alignment = data_alignment;
			this->data = NULL;

			this->pinned = pinned;

			//determine the type
			if(std::is_same<T,int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_INT;
			}
			else if(std::is_same<T,unsigned int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_UINT;
			}
			else if(std::is_same<T,char>::value == true)
			{
				this->data_type = SH_DATA_TYPE_CHAR;
			}
			else if(std::is_same<T,unsigned char>::value == true)
			{
				this->data_type = SH_DATA_TYPE_UCHAR;
			}
			else if(std::is_same<T,long int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONG;
			}
			else if(std::is_same<T,long long int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONGLONG;
			}
			else if(std::is_same<T,unsigned long>::value == true)
			{
				this->data_type = SH_DATA_TYPE_ULONG;
			}
			else if(std::is_same<T,unsigned long long>::value == true)
			{
				this->data_type = SH_DATA_TYPE_ULONGLONG;
			}
			else if(std::is_same<T,float>::value == true)
			{
				this->data_type = SH_DATA_TYPE_FLOAT;
			}
			else if(std::is_same<T,double>::value == true)
			{
				this->data_type = SH_DATA_TYPE_DOUBLE;
			}
			else if(std::is_same<T,long double>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONG_DOUBLE;
			}
			else if(std::is_same<T,unsigned short>::value == true)
			{
				this->data_type = SH_DATA_TYPE_USHORT;
			}
			else if(std::is_same<T,short>::value == true)
			{
				this->data_type = SH_DATA_TYPE_SHORT;
			}
			else
			{
				this->data_type = SH_DATA_TYPE_ABSTRACT;
			}
		}

        Matrix2(const Matrix2<T>& matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;

			this->data = NULL;
			this->data_type = matrix.getDataType();
			this->data_alignment = matrix.getDataAlignment();
			this->pinned = matrix.getPinned();

			this->initMatrix(matrix.getRowCount(), matrix.getColumnCount());

			//copy data
			//memcpy(this->data, matrix.getDataPtr(),  matrix.getRowCount()* matrix.getColumnCount() * sizeof(T));

			//copy data
			for(unsigned int i=0;i<this->getRowCount();i++)
			{
				for(unsigned int j=0;j<this->getColumnCount();j++)
				{
					this->setData(i,j, matrix.getData(i,j) );
				}
			}
		}


        Matrix2(Matrix2<T>* matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;

			this->data = NULL;
			this->data_type = matrix->getDataType();
			this->data_alignment = matrix->getDataAlignment();
			this->pinned = matrix->getPinned();

			this->initMatrix(matrix->getRowCount(), matrix->getColumnCount());

			//copy data
			//memcpy(this->data, matrix->getDataPtr(),  matrix->getRowCount()* matrix->getColumnCount() * sizeof(T));

			//copy data
			for(unsigned int i=0;i<this->getRowCount();i++)
			{
				for(unsigned int j=0;j<this->getColumnCount();j++)
				{
					this->setData(i,j, matrix->getData(i,j) );
				}
			}
		}

        Matrix2(Matrix2<T>* matrix, enum MATRIX_DATA_ALIGNMENT data_alignment, bool pinned = false)
        {
        	this->m_columns = 0;
        	this->m_rows = 0;

        	this->data = NULL;
        	this->data_type = matrix->getDataType();
        	this->data_alignment = data_alignment;
        	this->pinned = pinned;

        	this->initMatrix(matrix->getRowCount(), matrix->getColumnCount());

        	//copy data
        	for(unsigned int i=0;i<this->getRowCount();i++)
        	{
        		for(unsigned int j=0;j<this->getColumnCount();j++)
        		{
        			this->setData(i,j, matrix->getData(i,j) );
        		}
        	}

        }


        virtual ~Matrix2()
        {
        	if(this->pinned == false)
        	{

        		DELETE_ARRAY_NULL_CHECKING(this->data);

        	}
        	else
        	{
        		if(this->data != NULL)
				{
					munlock(this->data,sizeof(T)*this->m_columns*this->m_rows);
					delete this->data;
				}
        	}
        }

        /**
         * Keep in mind that the [][] approach induces an overhead since for each
         * request a new object will be instantiated.
         **/
        inline Matrix2RowPointer operator[](unsigned int row)
		{
        	return Matrix2RowPointer(this,row);
		}

        /**
		 * Keep in mind that the [][] approach induces an overhead since for each
		 * request a new object will be instantiated.
		 **/
		inline const Matrix2RowPointer operator[](unsigned int row) const
		{
			return Matrix2RowPointer(this,row);
		}

        Matrix2<T>& operator =(const Matrix2<T>& matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;

			DELETE_ARRAY_NULL_CHECKING(this->data);
			this->data_type = matrix.getDataType();
			this->data_alignment = matrix.getDataAlignment();
			this->pinned = matrix.getPinned();

			this->initMatrix(matrix.getRowCount(), matrix.getColumnCount());

			//copy data
			//memcpy(this->data, matrix.getDataPtr(),  matrix.getRowCount()* matrix.getColumnCount() * sizeof(T));

			for(unsigned int i=0;i<this->m_rows;++i)
			{
				for(unsigned int j=0;j<this->m_columns;++j)
				{
					this->setData(i,j,const_cast<const T&>(matrix.getData(i,j)) );
				}
			}

			return *this;
		}

        Matrix2<T>& operator =(const Matrix2<T>* matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;

			DELETE_ARRAY_NULL_CHECKING(this->data);
			this->data_type = matrix->getDataType();
			this->data_alignment = matrix->getDataAlignment();
			this->pinned = matrix->getPinned();

			this->initMatrix(matrix->getRowCount(), matrix->getColumnCount());

			//copy data
			//memcpy(this->data, matrix->getDataPtr(),  matrix->getRowCount()* matrix->getColumnCount() * sizeof(T));

			for(unsigned int i=0;i<this->m_rows;++i)
			{
				for(unsigned int j=0;j<this->m_columns;++j)
				{
					this->setData(i,j,const_cast<const T&>(matrix->getData(i,j)) );
				}
			}

			return *this;
		}

        bool getPinned() const
        {
        	return this->pinned;
        }

        CLASS_RTTI(Lazarus::Matrix2)

        void serialize()
        {
        	registerElement<unsigned int>(2);
        	registerElement<enum MATRIX_DATA_ALIGNMENT>(1);
        	registerElement<enum SH_DATA_TYPE>(1);
        	registerUCharA(m_rows*m_columns*sizeof(T));

			allocateBuffer();

			addElement<enum MATRIX_DATA_ALIGNMENT>(data_alignment);
			addElement<enum SH_DATA_TYPE>(data_type);
			addUCharA((unsigned char*)data,m_rows*m_columns*sizeof(T));
			addElement<unsigned int>(m_rows);
			addElement<unsigned int>(m_columns);
        }

        void deserialize()
        {
        	//free any data
        	DELETE_NULL_CHECKING(data);

        	m_columns = getElement<unsigned int>();
        	m_rows = getElement<unsigned int>();

        	unsigned long long size;
        	data = (T*)getUCharA(size);

			data_type = getElement<enum SH_DATA_TYPE>();
			data_alignment = getElement<enum MATRIX_DATA_ALIGNMENT>();

        	resetBuffer();
        }

        /**
         * On a call to this method the matrix will be resized to the specified dimensions. Any previously
         * allocated internal memory will be deleted, keep in mind that in case of pointers the corresponding
         * objects won't be deleted.For truly large matrices set 'parallel' true.
         */
        void initMatrix(unsigned int rows, unsigned int columns, bool parallel = false)
        {
        	if(rows == 0 || columns == 0)
        	{
        		return;
        	}

        	if(this->pinned == false)
        	{
        		// release memory if already allocated
        		DELETE_ARRAY_NULL_CHECKING(this->data);

        		// reserve memory
        		this->data = new T[rows*columns];
        		this->m_rows = rows;
        		this->m_columns = columns;
        	}
        	else
        	{

        		// release memory if already allocated
        		if(this->data != NULL)
        		{
        			munlock(this->data,sizeof(T)*this->m_columns*this->m_rows);
        			delete this->data;
        		}

        		// reserve memory and lock it
        		this->data = new T[rows*columns];
        		mlock(this->data,sizeof(T)*rows*columns);

        		this->m_rows = rows;
        		this->m_columns = columns;

        	}

        	resetMatrix(parallel);
        }

        /**
		 * On a call to this method the matrix will be resized to the specified dimensions. Any previously
		 * allocated internal memory will be deleted, keep in mind that in case of pointers the corresponding
		 * objects won't be deleted. The matrix will be initialized to all ZERO! If 'large' is set an optimized
         * subroutine will be used to initialize the entries, use this only for LARGE dims! For truly large matrices
         * set 'parallel' true.
		 */
		void initMatrixZERO(unsigned int rows, unsigned int columns, bool large = false, bool parallel = false)
		{
			if(rows == 0 || columns == 0)
			{
				return;
			}

			if(this->pinned == false)
			{
				// release memory if already allocated
				DELETE_ARRAY_NULL_CHECKING(this->data);

				// reserve memory
				this->data = new T[rows*columns];
				this->m_rows = rows;
				this->m_columns = columns;
			}
			else
			{

				// release memory if already allocated
				if(this->data != NULL)
				{
					munlock(this->data,sizeof(T)*this->m_columns*this->m_rows);
					delete this->data;
				}

				// reserve memory and lock it
				this->data = new T[rows*columns];
				mlock(this->data,sizeof(T)*rows*columns);

				this->m_rows = rows;
				this->m_columns = columns;

			}

			globalSetMatrix(0,large,parallel);
		}

		/**
		 * Will iterate over all entries and set them to T(). Use parallel for large matrices!
		 * */
        void resetMatrix( bool parallel = false )
        {
        	globalSetMatrixVal(T(), parallel);
        }

        /**
         * This is a fast method for setting the initial value, if 'large' is set an optimized
         * subroutine will be used to initialize the entries! For truly large matrices set 'parallel'
         * true. The optimization heuristic works best if the amount of columns largely exceeds the amount of
         * rows or vice versa.
         * Keep in mind that this routine will simply iterate in byte-wise manner over the underlying
         * array and set each byte to 'val'!!
         */
        void globalSetMatrix(unsigned char val, bool large = false, bool parallel = false)
		{
        	if(large == true)//set each row via memcpy
        		if(parallel == true)
        			if(m_columns >= m_rows)
        				Toolbox::setArrayValParallel(val, (unsigned char*)this->data, sizeof(T)*m_rows*m_columns, sizeof(T)*m_columns);
        			else
        				Toolbox::setArrayValParallel(val, (unsigned char*)this->data, sizeof(T)*m_rows*m_columns, sizeof(T)*m_rows);
        		else
        			if(m_columns >= m_rows)
						Toolbox::setArrayVal(val, (unsigned char*)this->data, sizeof(T)*m_rows*m_columns, sizeof(T)*m_columns);
					else
						Toolbox::setArrayVal(val, (unsigned char*)this->data, sizeof(T)*m_rows*m_columns, sizeof(T)*m_rows);
			else
				memset(this->data,val,sizeof(T)*m_rows*m_columns);
		}

        /**
		 * This is a relatively slow method for setting the initial value, yet it should be used
		 * for non-primitive datatypes or primitive types which exceed char size.
		 * Use parallel for large matrices!
		 */
		void globalSetMatrixVal(const T& val, bool parallel = false)
		{
			if(parallel == true)
				for(unsigned int i=0; i < this->m_columns * this->m_rows; ++i)
				{
					this->data[i] = val;
				}
			else
				#pragma omp parallel for
				for(unsigned int i=0; i < this->m_columns * this->m_rows; ++i)
				{
					this->data[i] = val;
				}
		}

		/**
		 * Returns the specified submatrix.
		 * */
		Matrix2<T>* getSubMatrix(unsigned int start_row, unsigned int start_column,
				unsigned int rows, unsigned int columns) const
		{
			Matrix2<T>* output = new Matrix2<T>(this->data_alignment);

			output->initMatrix(rows,columns);

			//copy the data
			if(this->data_alignment == Matrix::MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				for(unsigned int i=0;i<rows;++i)
				{
					for(unsigned int j=0;j<columns;++j)
					{
						output->setData(i,j, this->getData(start_row+i,start_column+j));
					}
				}
			}
			else
			{
				for(unsigned int j=0;j<columns;++j)
				{
					for(unsigned int i=0;i<rows;++i)
					{
						output->setData(i,j, this->getData(start_row+i,start_column+j));
					}
				}
			}

			return output;
		}

		/**
		 * Returns a copy of the matrix with added rows and columns, each matrix element is set to val.
		 * */
		Matrix2<T>* getPaddedMatrix(unsigned int rows, unsigned int columns, const T& val) const
		{
			Matrix2<T>* output = new Matrix2<T>(this->data_alignment);

			output->initMatrix(this->m_rows + rows,this->m_columns + columns);
			output->globalSetMatrixVal(val);

			//copy the data
			if(this->data_alignment == Matrix::MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				for(unsigned int i=0;i<this->m_rows;++i)
				{
					for(unsigned int j=0;j<this->m_columns;++j)
					{
						output->setData(i, j, this->getData(i,j));
					}
				}
			}
			else//column major
			{
				for(unsigned int j=0;j<this->m_columns;++j)
				{
					for(unsigned int i=0;i<this->m_rows;++i)
					{
						output->setData(i, j, this->getData(i,j));
					}
				}
			}

			return output;
		}

		/**
		 * returns a submatrix by removing row 'row' and column 'column' from the current matrix.
		 * */
		Matrix2<T>* removeRowColumn(unsigned int row, unsigned int column)
		{
			Matrix2<T>* output = new Matrix2<T>(this->data_alignment);
			output->initMatrix(m_rows-1,m_columns-1);

			int v_row = 0;
			int v_column = 0;

			for(unsigned int a=0; a < m_rows; a++)
			{
				if(a==row)
				{
					v_row = -1;
					continue;
				}

				for(unsigned int b=0; b < m_columns; b++)
				{

					if(b==column)
					{
						v_column = -1;
						continue;
					}

					output->setData(a+v_row, b+v_column, this->getData(a, b));

				}

				v_column = 0;

			}

			return output;
		}

		/**
		 * Calculates the matrix determinant. Returns 0 in case of abstract matrices.
		 * */
		template<typename U>
		U determinant()
		{
			U det = U();

			if(this->data_type != SH_DATA_TYPE_ABSTRACT)
			{
				det = determinant_<U>(this);
			}
			else
			{
				printf("ERROR: can not calculate determinant of abstract matrix\n");
			}

			return det;
		}

		/**
		 * Calculates the inverse of the matrix. Returns null if matrix is not invertible or of abstract type.
		 * */
		template<typename U>
		Matrix2<T>* getInverse()
		{
			Matrix2<T>* inv = NULL;
			U det = this->determinant<U>();

			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not calculate inverse of an abstract matrix\n");
				return inv;
			}

			//determinant = 0
			if(det == (U)0)
			{
				printf("ERROR: matrix is not invertible\n");
				return inv;
			}

			//not quadratic
			if(m_rows != m_columns)
			{
				printf("ERROR: matrix is not invertible\n");
				return inv;
			}



			for(int i=0;i<m_rows;++i)
			{
				for(int j=0;j<m_columns;++j)
				{
					Matrix2<T>* minor_ij = this->removeRowColumn(i, j);
					U det_minor = minor_ij->determinant<U>();
					inv->setData(j, i, (int)( std::pow(-1, i+1+j+1)*(det_minor/det) ) );//implicit transpose, thus inverse

					delete minor_ij;
				}
			}

			return inv;
		}


		/**
		 * Scales entries with a, only applicable for non-abstract matrices. Leaves matrix unchanged otherwise.
		 * */
		void scale(T a)
		{
			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not scale an abstract matrix\n");
				return;
			}

			for(int i=0;i<m_rows;++i)
			{
				for(int j=0;j<m_columns;++j)
				{
					this->setData(i, j, this->getData(i, j)*a);
				}
			}
		}

		/**
		 * A pointwise multiplication of the matrix with matrix A.
		 * */
		void pointMultiply(Lazarus::Matrix2<T>* A)
		{
			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not pointwise multiply an abstract matrix\n");
				return;
			}

			if(this->m_rows != A->getRowCount() || this->m_columns != A->getColumnCount())
			{
				printf("ERROR: can not pointwise multiply with matrix of different size\n");
				return;
			}

			for(unsigned int i=0;i<m_rows;++i)
			{
				for(unsigned int j=0;j<m_columns;++j)
				{
					this->setData(i,j, this->getData(i,j) * A->getData(i,j) );
				}
			}
		}

		/**
		 * A pointwise multiplication of the matrix with matrix A.
		 * */
		void pointMultiply(const Lazarus::Matrix2<T>& A)
		{
			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not pointwise multiply an abstract matrix\n");
				return;
			}

			if(this->m_rows != A.getRowCount() || this->m_columns != A.getColumnCount())
			{
				printf("ERROR: can not pointwise multiply with matrix of different size\n");
				return;
			}

			for(unsigned int i=0;i<m_rows;++i)
			{
				for(unsigned int j=0;j<m_columns;++j)
				{
					this->setData(i,j, this->getData(i,j) * A.getData(i,j) );
				}
			}
		}

		/**
		 * A pointwise division of the matrix with matrix A.
		 * */
		void pointDivide(Lazarus::Matrix2<T>* A)
		{
			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not pointwise divide an abstract matrix\n");
				return;
			}

			if(this->m_rows != A->getRowCount() || this->m_columns != A->getColumnCount())
			{
				printf("ERROR: can not pointwise divide with matrix of different size\n");
				return;
			}

			for(unsigned int i=0;i<m_rows;++i)
			{
				for(unsigned int j=0;j<m_columns;++j)
				{
					this->setData(i,j, this->getData(i,j) / A->getData(i,j) );
				}
			}
		}

		/**
		 * A pointwise division of the matrix with matrix A.
		 * */
		void pointDivide(const Lazarus::Matrix2<T>& A)
		{
			//abstract matrix
			if(this->data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can not pointwise divide an abstract matrix\n");
				return;
			}

			if(this->m_rows != A.getRowCount() || this->m_columns != A.getColumnCount())
			{
				printf("ERROR: can not pointwise divide with matrix of different size\n");
				return;
			}

			for(unsigned int i=0;i<m_rows;++i)
			{
				for(unsigned int j=0;j<m_columns;++j)
				{
					this->setData(i,j, this->getData(i,j) / A.getData(i,j) );
				}
			}
		}


        /**
         * this should only be used for debug purposes and especially for small matrices, thus we don't care
         * about the switch within each iteration of the inner most for loop.
         */
        virtual void printData() const
        {
        	for(unsigned int i=0;i<m_rows;++i)
			{
			  for(unsigned int j=0;j<m_columns;++j)
			  {
				  switch(this->data_type)
				  {
					  case SH_DATA_TYPE_DOUBLE:
						  printf("%F ", *((double*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_FLOAT:
						  printf("%f ", *((float*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_LONG_DOUBLE:
						  printf("%Lf ", *((long double*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_INT:
						  printf("%d ", *((int*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_LONGLONG:
						  printf("%lld ", *((long long int*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_ULONGLONG:
						  printf("%llu ", *((unsigned long long*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_LONG:
						  printf("%ld ", *(( long int*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_ULONG:
						  printf("%lu ", *((unsigned long*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_UINT:
						  printf("%u ", *((unsigned int*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_UCHAR:
						  printf("%u ", *((unsigned char*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_CHAR:
						  printf("%d ", *((char*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_SHORT:
						  printf("%hd ", *((short*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_USHORT:
						  printf("%hu ", *((unsigned short*)&getData(i,j)));
						  break;
					  case SH_DATA_TYPE_ABSTRACT:
						  printf("ABSTRACT ");
						  break;
					  case SH_DATA_TYPE_UNSPECIFIED:
						  printf(" UNKNOWN ");
						  break;
				  }
			  }
			  printf("\n");
			}
        }

        void setData(unsigned int i, unsigned int j, const T& value)
        {
        	if(i>=m_rows)
			{
				printf("ERROR: setData row %d not available\n",i);
				return;
			}
			if(j>=m_columns)
			{
				printf("ERROR: setData column %d not available\n",j);
				return;
			}

        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
        	{
        		this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)] = value;
        	}
        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
        	{
        		this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)] = value;
        	}

        }

        inline T& getData(unsigned int i, unsigned int j) const
        {
        	//T output=0;

        	if(i>=m_rows)
        	{
        		printf("ERROR: getData row %d not available\n",i);
        		return this->data[0];
        	}
        	if(j>=m_columns)
			{
				printf("ERROR: getData column %d not available\n",j);
				return this->data[0];
			}

        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
        	{
        		//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)];
        		return this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)];
        	}
        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
        	{
        		//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)];
        		return this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)];
        	}

        	return this->data[0];

        	//return output;
        }



        T& operator ()(unsigned int& i, unsigned int& j)
		{
			//T output=0;

        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)];
				return this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)];
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)];
				return this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)];
			}

			return this->data[0];

			//return output;
		}

        inline T* getDataRef(unsigned int i, unsigned int j)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				output = this->data + IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows);
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				output = this->data + IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns);
			}

			return output;
		}

        /*
         * This method returns a pointer to row i of the underlying data array.
         * I.e. one can return a pointer to the i-th row of a row-major matrix
         */
        inline T* getRawDataRow(unsigned int i)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				printf("ERROR: can not return row reference as the matrix is in column major format\n");
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				output = &(this->data[ i*this->m_columns ] );
			}

			return output;
		}

        /*
		 * This method returns a pointer to column i of the underlying data array.
		 * I.e. one can return a pointer to the i-th column of a column-major matrix
		 */
		inline T* getRawDataColumn(unsigned int i)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				printf("ERROR: can not return column reference as the matrix is in row major format\n");
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				output = &(this->data[ i*this->m_rows ] );
			}

			return output;
		}


        T* getDataPtr() const
        {
        	return this->data;
        }

        unsigned int getRowCount() const
        {
        	return this->m_rows;
        }

        unsigned int getColumnCount() const
        {
        	return this->m_columns;
        }

        unsigned int rows() const
		{
			return this->m_rows;
		}

		unsigned int columns() const
		{
			return this->m_columns;
		}

        void readMatrixFromFile(const std::string& filename)
        {
        	setlocale(LC_ALL,"C");

        	FILE* pFile = fopen(filename.c_str(),"r");
        	std::string s;

        	//read header and init data structures
        	// from now on we assume a correct format of the input file !!
        	std::string rows="";
        	std::string columns="";
        	readCSVValue(&pFile,&rows);
        	readCSVValue(&pFile,&columns);
        	this->initMatrix( Toolbox::stringToInt( rows ),Toolbox::stringToInt( columns ) );

        	//load data into matrix
        	for(unsigned int i=0;i<this->m_rows;i++)
        	{
        			for(unsigned int j=0;j<this->m_columns;j++)
        			{
        				readCSVValue(&pFile,&s); //read token
        				#ifdef DEBUG
        				printf("setting number %s \n",s.c_str());
        				#endif
        				this->setData(i,j, Toolbox::stringToDouble( s ) );
        			}
        	}

        	fclose(pFile);
        }

        void readMatrixFromBinaryFile(const std::string& filename)
        {
        	FILE* pFile = fopen(filename.c_str(),"rb");
        	std::string s;

        	//read header and init data structures
        	// from now on we assume a correct format of the input file !!
        	std::string rows="";
        	std::string columns="";
        	readCSVValue(&pFile,&rows);
        	readCSVValue(&pFile,&columns);
        	this->initMatrix( Toolbox::stringToInt( rows ),Toolbox::stringToInt( columns ) );

        	//load data into matrix
        	fread(this->data, this->m_rows*this->m_columns,sizeof(T), pFile);

        	fclose(pFile);
        }

        void writeMatrixToFile(const std::string& filename) const
        {
        	if(data_type == SH_DATA_TYPE_ABSTRACT)
        	{
        		printf("ERROR: can't save abstract matrix in CSV format, use binary export methods\n");
        	}

        	setlocale(LC_ALL,"C");

        	FILE* pFile = fopen(filename.c_str(),"w");
        	std::string s;

        	//header
        	s = Toolbox::intToString( this->m_rows );
        	fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        	fputc(',',pFile);
        	s = Toolbox::intToString( this->m_columns );
        	fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        	fputc(';',pFile);
        	fputc('\n',pFile);

        	//data
        	for(unsigned int i=0;i<this->m_rows;i++)
        	{
        			for(unsigned int j=0;j<this->m_columns;j++)
        			{
        				if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
        				{
        					s = Toolbox::doubleToString( this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM2(i,j,this->m_rows)] );
        				}
        				if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
        				{
        					s = Toolbox::doubleToString( this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM2(i,j,this->m_columns)] );
        				}
        				fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        				if(j+1<this->m_columns) fputc(',',pFile);
        			}
        			fputc(';',pFile);
        			fputc('\n',pFile);
        	}
        	fclose(pFile);
        }

        void writeMatrixToBinaryFile(const std::string& filename) const
		{
			FILE* pFile = fopen(filename.c_str(),"wb");
			std::string s;

			//header
			s = Toolbox::intToString( this->m_rows );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(',',pFile);
			s = Toolbox::intToString( this->m_columns );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(';',pFile);
			fputc('\n',pFile);

			//data
			fwrite(this->data, this->m_columns*this->m_rows,sizeof(T), pFile);

			fclose(pFile);
		}


        void exportMatrix(const std::string& filename)
        {
        	if(data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can't save abstract matrix in CSV format, use binary export methods\n");
			}

        	FILE* pFile = fopen(filename.c_str(),"w");
        	std::string s;

        	for(unsigned int i=0;i<this->m_rows;i++)
        	{
        			for(unsigned int j=0;j<this->m_columns;j++)
        			{
        				s = Toolbox::doubleToString( (double)getData(i,j) );
        				fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        				if(j+1<this->m_columns) fputc(',',pFile);
        			}
        			fputc(';',pFile);
        			fputc('\n',pFile);
        	}
        	fclose(pFile);

        }

        void exportMatrixBinary(const std::string& filename)
		{

			FILE* pFile = fopen(filename.c_str(),"wb");

			//data
			fwrite(this->data, this->m_columns*this->m_rows,sizeof(T), pFile);

			fclose(pFile);

		}

        enum MATRIX_DATA_ALIGNMENT getDataAlignment() const
        {
        	return this->data_alignment;
        }

        enum SH_DATA_TYPE getDataType() const
        {
        	return this->data_type;
        }


protected:
        unsigned int m_rows;
        unsigned int m_columns;
        T* data;
        bool pinned;

        // reads a csv-token, returns false if terminated by , and true if terminated by ; or EOF
        bool readCSVValue(FILE** pFile, std::string* s)
        {

        	char c[2];
        	c[1] = '\0';
        	*s = ""; //reset string

        	//read chars until ,  ; or EOF found
        	while(1){
        		c[0] = fgetc(*pFile);
        		if(c[0]==',') return false;
        		if(c[0]==';' || c[0]==EOF)
        			{
        			fgetc(*pFile); //read ahead one char, i.e. skip line break. Uncomment this if linebreaks are not being used after ;
        			return true;
        			}
        		#ifdef DEBUG
        		printf("appending %s to %s \n",(char*)&c,(*s).c_str());
        		#endif
        		s->append(c);
        	}

        }


        /**
         * Helper method for determinant calculation. Only applicable for non-abstract template parameters.
         * */
        template<typename U>
		U determinant_(Matrix2<T>* mat)
		{

			int sum=0;
		    int s;

		    if(mat->getColumnCount()==1 && mat->getRowCount()==1)
		    {
		    	//bottom case of recursion. size 1 matrix determinant is itself.
		    	return(mat->getData(0, 0));
		    }

		    for(unsigned int i=0;i<mat->getRowCount();i++)
		    {
				//finds determinant using row-by-row expansion
				Matrix2<T>* submat = mat->removeRowColumn(i,0);

				//submat->printData();

				if(i%2==0)
				{
					s=1;
				}
				else
				{
					s=-1;
				}

				sum += s * mat->getData(i, 0)*(determinant_<U>(submat));

				//printf("%u: %d \n",i,sum);

				delete submat;
		    }

		    //printf("final %d \n",sum);

			return(sum); //returns determinant value. once stack is finished, returns final determinant.
		}

};

//******************* 3 dims *******************************

template<class T> class Matrix3 : public Matrix{
public:
		/**
		 * This class is merely a state container in order to use the [][][]
		 * operator on a matrix, i.e. mat[][] returns a row-column pointer object X
		 * which has the desired row and column >indices< temporarily saved.
		 * X[] will deliver the row pointer.
		 * */
		class Matrix3RowColumnPointer
		{
			friend class Matrix3<T>;
		public:
			T& operator[](unsigned int level)
			{
				if(m_row>=mp_matrix->m_rows)
				{
					printf("ERROR: getData row %d not available\n",m_row);
					return mp_matrix->data[0];
				}
				if(m_col>=mp_matrix->m_columns)
				{
					printf("ERROR: getData column %d not available\n",m_col);
					return mp_matrix->data[0];
				}
				if(level>=mp_matrix->m_levels)
				{
					printf("ERROR: getData level %d not available\n",level);
					return mp_matrix->data[0];
				}

				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(m_row,m_col,level,mp_matrix->m_rows,mp_matrix->m_columns)];
				}
				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
					return  mp_matrix->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(m_row,m_col,level,mp_matrix->m_rows,mp_matrix->m_columns)];
				}

				return mp_matrix->data[0];
			}

			const T& operator[](unsigned int level) const
			{
				if(m_row>=mp_matrix->m_rows)
				{
					printf("ERROR: getData row %d not available\n",m_row);
					return mp_matrix->data[0];
				}
				if(m_col>=mp_matrix->m_columns)
				{
					printf("ERROR: getData column %d not available\n",m_col);
					return mp_matrix->data[0];
				}
				if(level>=mp_matrix->m_levels)
				{
					printf("ERROR: getData level %d not available\n",level);
					return mp_matrix->data[0];
				}

				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
					return mp_matrix->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(m_row,m_col,level,mp_matrix->m_rows,mp_matrix->m_columns)];
				}
				if(mp_matrix->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
				{
					//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
					return  mp_matrix->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(m_row,m_col,level,mp_matrix->m_rows,mp_matrix->m_columns)];
				}

				return mp_matrix->data[0];
			}

		private:
			Matrix3RowColumnPointer(Matrix3<T>* mat,int row, int column)
			{
				mp_matrix = mat;
				m_col = column;
				m_row = row;
			}
			Matrix3<T>* mp_matrix;//external ref
			unsigned int m_col;
			unsigned int m_row;

		};


		/**
		 * This class is merely a state container in order to use the [][][]
		 * operator on a matrix, i.e. mat[][] returns a row-column pointer object X
		 * which has the desired row and column >indices< temporarily saved.
		 * X[] will deliver the row pointer.
		 * */
		class Matrix3RowPointer
		{
			friend class Matrix3<T>;
			friend class Matrix3RowColumnPointer;
		public:
			Matrix3RowColumnPointer operator[](unsigned int column)
			{
				return Matrix3LevelPointer(mp_matrix,m_row,column);
			}

			const Matrix3RowColumnPointer operator[](unsigned int column) const
			{
				return Matrix3LevelPointer(mp_matrix,m_row,column);
			}

		private:
			Matrix3RowPointer(Matrix3<T>* mat, unsigned int row)
			{
				mp_matrix = mat;
				m_row = row;
			}
			Matrix3<T>* mp_matrix;//external ref
			unsigned int m_row;

		};

        Matrix3(enum MATRIX_DATA_ALIGNMENT data_alignment = MATRIX_DATA_ALIGNMENT_ROW_MAJOR, bool pinned = false)
		{
			this->m_columns = 0;
			this->m_rows = 0;
			this->m_levels = 0;

			this->data_alignment = data_alignment;
			this->data = NULL;

			this->pinned = pinned;

			//determine the type
			if(std::is_same<T,int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_INT;
			}
			else if(std::is_same<T,unsigned int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_UINT;
			}
			else if(std::is_same<T,char>::value == true)
			{
				this->data_type = SH_DATA_TYPE_CHAR;
			}
			else if(std::is_same<T,unsigned char>::value == true)
			{
				this->data_type = SH_DATA_TYPE_UCHAR;
			}
			else if(std::is_same<T,long int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONG;
			}
			else if(std::is_same<T,long long int>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONGLONG;
			}
			else if(std::is_same<T,unsigned long>::value == true)
			{
				this->data_type = SH_DATA_TYPE_ULONG;
			}
			else if(std::is_same<T,unsigned long long>::value == true)
			{
				this->data_type = SH_DATA_TYPE_ULONGLONG;
			}
			else if(std::is_same<T,float>::value == true)
			{
				this->data_type = SH_DATA_TYPE_FLOAT;
			}
			else if(std::is_same<T,double>::value == true)
			{
				this->data_type = SH_DATA_TYPE_DOUBLE;
			}
			else if(std::is_same<T,long double>::value == true)
			{
				this->data_type = SH_DATA_TYPE_LONG_DOUBLE;
			}
			else if(std::is_same<T,unsigned short>::value == true)
			{
				this->data_type = SH_DATA_TYPE_USHORT;
			}
			else if(std::is_same<T,short>::value == true)
			{
				this->data_type = SH_DATA_TYPE_SHORT;
			}
			else
			{
				this->data_type = SH_DATA_TYPE_ABSTRACT;
			}
		}

        Matrix3(Matrix3<T>* matrix, enum MATRIX_DATA_ALIGNMENT data_alignment, bool pinned = false)
        {
        	this->m_columns = 0;
        	this->m_rows = 0;
        	this->m_levels = 0;

        	this->data = NULL;
        	this->data_type = matrix->getDataType();
        	this->data_alignment = data_alignment;
        	this->pinned = pinned;

        	this->initMatrix(matrix->getRowCount(), matrix->getColumnCount(), matrix->getLevelCount());

        	//copy data
        	for(unsigned int k=0;k<this->getLevelCount();k++)
        	{
				for(unsigned int i=0;i<this->getRowCount();i++)
				{
					for(unsigned int j=0;j<this->getColumnCount();j++)
					{
						this->setData(i,j,k, matrix->getData(i,j,k) );
					}
				}
        	}

        }

        Matrix3(Matrix3<T>* matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;
			this->m_levels = 0;

			this->data = NULL;
			this->data_type = matrix->getDataType();
			this->data_alignment = matrix->getDataAlignment();
			this->pinned = matrix->getPinned();

			this->initMatrix(matrix->getRowCount(), matrix->getColumnCount(), matrix->getLevelCount());

			//copy data
			//memcpy(this->data, matrix->getDataPtr(),  matrix->getRowCount()* matrix->getColumnCount() * matrix->getLevelCount() * sizeof(T));

			//copy data
			for(unsigned int k=0;k<this->getLevelCount();k++)
			{
				for(unsigned int i=0;i<this->getRowCount();i++)
				{
					for(unsigned int j=0;j<this->getColumnCount();j++)
					{
						this->setData(i,j,k, matrix->getData(i,j,k) );
					}
				}
			}
		}

        Matrix3(const Matrix3<T>& matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;
			this->m_levels = 0;

			this->data = NULL;
			this->data_type = matrix.getDataType();
			this->data_alignment = matrix.getDataAlignment();
			this->pinned = matrix.getPinned();

			this->initMatrix(matrix.getRowCount(), matrix.getColumnCount(), matrix.getLevelCount());

			//copy data
			//memcpy(this->data, matrix.getDataPtr(),  matrix.getRowCount()* matrix.getColumnCount() * matrix.getLevelCount() * sizeof(T));

			//copy data
			for(unsigned int k=0;k<this->getLevelCount();k++)
			{
				for(unsigned int i=0;i<this->getRowCount();i++)
				{
					for(unsigned int j=0;j<this->getColumnCount();j++)
					{
						this->setData(i,j,k, matrix.getData(i,j,k) );
					}
				}
			}

		}


        virtual ~Matrix3()
        {
        	if(this->pinned == false)
        	{

        		DELETE_ARRAY_NULL_CHECKING(this->data);

        	}
        	else
        	{
        		if(this->data != NULL)
				{
					munlock(this->data,sizeof(T)*this->m_columns*this->m_rows*this->m_levels);
					delete this->data;
				}
        	}
        }

        /**
		 * Keep in mind that the [][] approach induces an overhead since for each
		 * request a new object will be instantiated.
		 **/
		inline const Matrix3RowPointer operator[](int row) const
		{
			return Matrix3RowPointer(this,row);
		}

        Matrix3<T>& operator =(const Matrix3<T>& matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;
			this->m_levels = 0;

			DELETE_ARRAY_NULL_CHECKING(this->data);
			this->data_type = matrix.getDataType();
			this->data_alignment = matrix.getDataAlignment();
			this->pinned = matrix.getPinned();

			this->initMatrix(matrix.getRowCount(), matrix.getColumnCount(), matrix.getLevelCount());

			//copy data
			//memcpy(this->data, matrix.getDataPtr(),  matrix.getRowCount()* matrix.getColumnCount() * matrix.getLevelCount() * sizeof(T));

			for(unsigned int k=0;k<this->getLevelCount();k++)
			{
				for(unsigned int i=0;i<this->getRowCount();i++)
				{
					for(unsigned int j=0;j<this->getColumnCount();j++)
					{
						this->setData(i,j,k, const_cast<const T&>(matrix.getData(i,j,k)) );
					}
				}
			}

			return *this;
		}

        Matrix3<T>& operator =(const Matrix3<T>* matrix)
		{
			this->m_columns = 0;
			this->m_rows = 0;
			this->m_levels = 0;

			DELETE_ARRAY_NULL_CHECKING(this->data);
			this->data_type = matrix->getDataType();
			this->data_alignment = matrix->getDataAlignment();
			this->pinned = matrix->getPinned();

			this->initMatrix(matrix->getRowCount(), matrix->getColumnCount(), matrix->getLevelCount());

			//copy data
			//memcpy(this->data, matrix->getDataPtr(),  matrix->getRowCount()* matrix->getColumnCount() * matrix->getLevelCount() * sizeof(T));

			for(unsigned int k=0;k<this->getLevelCount();k++)
			{
				for(unsigned int i=0;i<this->getRowCount();i++)
				{
					for(unsigned int j=0;j<this->getColumnCount();j++)
					{
						this->setData(i,j,k, const_cast<const T&>(matrix->getData(i,j,k)) );
					}
				}
			}

			return *this;
		}

        bool getPinned() const
		{
			return this->pinned;
		}

        CLASS_RTTI(Lazarus::Matrix3)

        void serialize()
		{
			registerElement<unsigned int>(3);
        	registerElement<enum MATRIX_DATA_ALIGNMENT>(1);
        	registerElement<enum SH_DATA_TYPE>(1);
			registerUCharA(m_rows*m_columns*m_levels*sizeof(T));

			allocateBuffer();

			addElement<enum MATRIX_DATA_ALIGNMENT>(data_alignment);
			addElement<enum SH_DATA_TYPE>(data_type);
			addUCharA((unsigned char*)data,m_rows*m_columns*m_levels*sizeof(T));
			addElement<unsigned int>(m_rows);
			addElement<unsigned int>(m_columns);
			addElement<unsigned int>(m_levels);
		}

		void deserialize()
		{
			//free any data
			DELETE_NULL_CHECKING(data);

			m_levels = getElement<unsigned int>();
			m_columns = getElement<unsigned int>();
			m_rows = getElement<unsigned int>();

			unsigned long long size;
			data = (T*)getUCharA(size);

			data_type = getElement<enum SH_DATA_TYPE>();
			data_alignment = getElement<enum MATRIX_DATA_ALIGNMENT>();

			resetBuffer();
		}

        /**
		 * On a call to this method the matrix will be resized to the specified dimensions. Any previously
		 * allocated internal memory will be deleted, keep in mind that in case of pointers the corresponding
		 * objects won't be deleted. Another important fact is that this method merely resizes the matrix,
		 * the "slots" won't be initialized to any default value.
		 */
        void initMatrix(int rows, int columns, int levels)
        {
        	if(rows == 0 || columns == 0 || levels == 0)
        	{
        		return;
        	}

        	if(this->pinned == false)
        	{
        		// release memory if already allocated
        		DELETE_ARRAY_NULL_CHECKING(this->data);

        		// reserve memory
        		this->data = new T[rows*columns*levels];
        		this->m_rows = rows;
        		this->m_columns = columns;
        		this->m_levels = levels;
        	}
        	else
        	{

        		// release memory if already allocated
        		if(this->data != NULL)
        		{
        			munlock(this->data,sizeof(T)*this->m_columns*this->m_rows*this->m_levels);
        			delete this->data;
        		}

        		// reserve memory and lock it
        		this->data = new T[rows*columns*levels];
        		mlock(this->data,sizeof(T)*rows*columns*levels);

        		this->m_rows = rows;
        		this->m_columns = columns;
        		this->m_levels = levels;

        	}

        	resetMatrix();
        }

        void resetMatrix()
		{
			//memset(this->data,0,sizeof(T)*this->m_columns*this->m_rows*this->m_levels);
        	globalSetMatrixVal(T());
		}

        /**
		 * This is a fast method for setting the initial value
		 */
        void globalSetMatrix(unsigned char val)
		{
			memset(this->data,val,sizeof(T)*this->m_columns*this->m_rows*this->m_levels);
		}


        /**
		 * This is a relatively slow method for setting the initial value
		 */
		void globalSetMatrixVal(T val)
		{
			for(unsigned int i=0; i < this->m_columns * this->m_rows * this->m_levels; ++i)
			{
				this->data[i] = val;
			}
		}

		/**
		 * Returns the specified submatrix
		 * */
		Matrix3<T>* getSubMatrix(unsigned int start_row, unsigned int start_column, unsigned int start_level,
				unsigned int rows, unsigned int columns, unsigned int levels)
		{
			Matrix3<T>* output = new Matrix3<T>(this->data_alignment);

			output->initMatrix(rows,columns,levels);

			//copy the data
			if(this->data_alignment == Matrix::MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				for(unsigned int k=0;k<levels;++k)
				{
					for(unsigned int i=0;i<rows;++i)
					{
						for(unsigned int j=0;j<columns;++j)
						{
								output->setData(i,j,k, this->getData(start_row+i,start_column+j,start_level+k));
						}
					}
				}
			}
			else
			{
				for(unsigned int k=0;k<levels;++k)
				{
					for(unsigned int j=0;j<columns;++j)
					{
						for(unsigned int i=0;i<rows;++i)
						{
								output->setData(i,j,k, this->getData(start_row+i,start_column+j,start_level+k));
						}
					}
				}
			}

			return output;
		}

		/**
		 * Returns a copy of the matrix with added levels, rows and columns, each matrix element is set to val.
		 * */
		Matrix3<T>* getPaddedMatrix(unsigned int levels, unsigned int rows, unsigned int columns, const T& val)
		{
			Matrix3<T>* output = new Matrix3<T>(this->data_alignment);

			output->initMatrix(this->m_rows + rows,this->m_columns + columns, this->m_levels + levels);
			output->globalSetMatrixVal(val);

			//copy the data
			if(this->data_alignment == Matrix::MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				for(unsigned int k=0;k< this->m_levels;++k)
				{
					for(unsigned int i=0;i<this->m_rows;++i)
					{
						for(unsigned int j=0;j<this->m_columns;++j)
						{
								output->setData(i, j, k, this->getData(i,j,k));
						}
					}
				}
			}
			else
			{
				for(unsigned int k=0;k< this->m_levels;++k)
				{
					for(unsigned int j=0;j<this->m_columns;++j)
					{
						for(unsigned int i=0;i<this->m_rows;++i)
						{
								output->setData(i, j, k, this->getData(i,j,k));
						}
					}
				}
			}

			return output;
		}


        void setData(unsigned int i, unsigned int j, unsigned int k, const T& value)
        {
        	if(i>=m_rows)
			{
				printf("ERROR: setData row %d not available\n",i);
				return ;
			}
			if(j>=m_columns)
			{
				printf("ERROR: setData column %d not available\n",j);
				return ;
			}
			if(k>=m_columns)
			{
				printf("ERROR: setData level %d not available\n",k);
				return ;
			}

        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
        	{
        		this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)] = value;
        	}
        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
        	{
        		this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)] = value;
        	}

        }

        inline T& getData(unsigned int i, unsigned int j, unsigned int k) const
        {
        	//T output=0;

        	if(i>=m_rows)
			{
				printf("ERROR: getData row %d not available\n",i);
				return this->data[0];
			}
			if(j>=m_columns)
			{
				printf("ERROR: getData column %d not available\n",j);
				return this->data[0];
			}
			if(k>=m_levels)
			{
				printf("ERROR: getData level %d not available\n",k);
				return this->data[0];
			}

        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
        	{
        		//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
        		return this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)];
        	}
        	if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
        	{
        		//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
        		return  this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)];
        	}

        	return this->data[0];
        	//return output;
        }


        T& operator () (unsigned int& i, unsigned int& j, unsigned int& k)
		{
			//T output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				//output = this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
				return this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)];
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				//output = this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->rows,this->columns)];
				return  this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)];
			}

			return T();

			//return output;
		}

        inline T* getDataRef(unsigned int i, unsigned int j, unsigned int k)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				output = &(this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)]);
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				output = &(this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)]);
			}

			return output;
		}

        /*
		 * This method returns a pointer to row i of the underlying data array.
		 * I.e. one can return a pointer to the i-th row of a row-major matrix,
		 * k determines the slice.
		 */
		inline T* getRawDataRow(unsigned int i, unsigned int k)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				printf("ERROR: can not return row reference as the matrix is in column major format\n");
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				output = &(this->data[ k*this->m_columns*this->m_rows + i*this->m_columns ] );
			}

			return output;
		}

		/*
		 * This method returns a pointer to column i of the underlying data array.
		 * I.e. one can return a pointer to the i-th column of a column-major matrix,
		 * k determines the slice.
		 */
		inline T* getRawDataColumn(unsigned int i, unsigned int k)
		{
			T* output=0;

			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
			{
				printf("ERROR: can not return column reference as the matrix is in row major format\n");
			}
			if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
			{
				output = &(this->data[ k*this->m_columns*this->m_rows + i*this->m_rows ] );
			}

			return output;
		}


        T* getDataPtr() const
        {
        	return this->data;
        }

        int getRowCount() const
        {
        	return this->m_rows;
        }

        int getColumnCount() const
        {
        	return this->m_columns;
        }

        int getLevelCount() const
		{
			return this->m_levels;
		}

        int rows() const
		{
			return this->m_rows;
		}

		int columns() const
		{
			return this->m_columns;
		}

		int levels() const
		{
			return this->m_levels;
		}

        /*
		 * this should only be used for debug purposes and especially for small matrices, thus we don't care
		 * about the switch within each iteration of the inner most for loop.
		 */
		void printData() const
		{
			for(unsigned int k=0;k<m_levels;++k)
			{
				for(unsigned int i=0;i<m_rows;++i)
				{
				  for(unsigned int j=0;j<m_columns;++j)
				  {
					  switch(this->data_type)
					  {
						  case SH_DATA_TYPE_DOUBLE:
							  printf("%F ", (double)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_FLOAT:
							  printf("%f ", (float)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_LONG_DOUBLE:
							  printf("%Lf ", (long double)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_INT:
							  printf("%d ", (int)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_LONGLONG:
							  printf("%lld ", (long long int)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_ULONGLONG:
							  printf("%llu ", (unsigned long long)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_LONG:
							  printf("%ld ", (long int)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_ULONG:
							  printf("%lu ", (unsigned long)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_UINT:
							  printf("%u ", (unsigned int)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_UCHAR:
							  printf("%c ", (unsigned char)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_CHAR:
							  printf("%d ", (char)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_SHORT:
							  printf("%hd ", (short)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_USHORT:
							  printf("%hu ", (unsigned short)getData(i,j,k));
							  break;
						  case SH_DATA_TYPE_ABSTRACT:
							  printf("ABSTRACT ");
							  break;
						  case SH_DATA_TYPE_UNSPECIFIED:
							  printf(" UNKNOWN ");
							  break;
					  }
				  }
				  printf("\n");
				}
				printf("\n****************\n");
			}
		}

        void readMatrixFromFile(const std::string& filename)
        {
        	setlocale(LC_ALL,"C");

        	FILE* pFile = fopen(filename.c_str(),"r");
        	std::string s;

        	//read header and init data structures
        	// from now on we assume a correct format of the input file !!
        	std::string rows="";
        	std::string columns="";
        	std::string levels="";
        	readCSVValue(&pFile,&rows);
        	readCSVValue(&pFile,&columns);
        	readCSVValue(&pFile,&levels);
        	this->initMatrix( Toolbox::stringToInt( rows ),Toolbox::stringToInt( columns ),Toolbox::stringToInt( levels ) );

        	//load data into matrix
        	for(unsigned int k=0;k<this->m_levels;k++)
			{
				for(unsigned int i=0;i<this->m_rows;i++)
				{
						for(unsigned int j=0;j<this->m_columns;j++)
						{
							readCSVValue(&pFile,&s); //read token
							#ifdef DEBUG
							printf("setting number %s \n",s.c_str());
							#endif
							this->setData(i,j,k, Toolbox::stringToDouble( s ) );
						}
				}
			}

        	fclose(pFile);
        }

        void readMatrixFromBinaryFile(const std::string& filename)
		{
			FILE* pFile = fopen(filename.c_str(),"rb");
			std::string s;

			//read header and init data structures
			// from now on we assume a correct format of the input file !!
			std::string rows="";
			std::string columns="";
			std::string levels="";
			readCSVValue(&pFile,&rows);
			readCSVValue(&pFile,&columns);
			readCSVValue(&pFile,&levels);
			this->initMatrix( Toolbox::stringToInt( rows ),Toolbox::stringToInt( columns ),Toolbox::stringToInt( levels ) );

			//load data into matrix
			fread(this->data, this->m_rows*this->m_columns*this->m_levels,sizeof(T), pFile);

			fclose(pFile);
		}

        void writeMatrixToFile(const std::string& filename) const
        {
        	if(data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can't save abstract matrix in CSV format, use binary export methods\n");
			}

        	setlocale(LC_ALL,"C");

        	FILE* pFile = fopen(filename.c_str(),"w");
        	std::string s;

        	//header
        	s = Toolbox::intToString( this->m_rows );
        	fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        	fputc(',',pFile);
        	s = Toolbox::intToString( this->m_columns );
        	fwrite(s.c_str(), s.length(),sizeof(char), pFile);
        	fputc(',',pFile);
        	s = Toolbox::intToString( this->m_levels );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(';',pFile);
        	fputc('\n',pFile);

        	//data
        	for(unsigned int k=0;k<this->m_levels;k++)
        	{
				for(unsigned int i=0;i<this->m_rows;i++)
				{
						for(unsigned int j=0;j<this->m_columns;j++)
						{
							if(this->data_alignment == MATRIX_DATA_ALIGNMENT_COLUMN_MAJOR)
							{
								s = Toolbox::doubleToString( this->data[IDX2ARRAYCOORDINATE_CM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)] );
							}
							if(this->data_alignment == MATRIX_DATA_ALIGNMENT_ROW_MAJOR)
							{
								s = Toolbox::doubleToString( this->data[IDX2ARRAYCOORDINATE_RM_NO_OFFSET_DIM3(i,j,k,this->m_rows,this->m_columns)] );
							}
							fwrite(s.c_str(), s.length(),sizeof(char), pFile);
							if(j+1<this->columns) fputc(',',pFile);
						}
						fputc(';',pFile);
						fputc('\n',pFile);
				}
        	}
        	fclose(pFile);
        }


        void writeMatrixToBinaryFile(const std::string& filename) const
		{
			FILE* pFile = fopen(filename.c_str(),"wb");
			std::string s;

			//header
			s = Toolbox::intToString( this->m_rows );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(',',pFile);
			s = Toolbox::intToString( this->m_columns );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(',',pFile);
			s = Toolbox::intToString( this->m_levels );
			fwrite(s.c_str(), s.length(),sizeof(char), pFile);
			fputc(';',pFile);
			fputc('\n',pFile);

			//data
			fwrite(this->data, this->m_columns*this->m_rows*this->m_levels,sizeof(T), pFile);

			fclose(pFile);
		}


        void exportMatrix(const std::string& filename)
        {
        	if(data_type == SH_DATA_TYPE_ABSTRACT)
			{
				printf("ERROR: can't save abstract matrix in CSV format, use binary export methods\n");
			}

        	FILE* pFile = fopen(filename.c_str(),"w");
        	std::string s;

        	for(unsigned int k=0;k<this->m_levels;k++)
			{
				for(unsigned int i=0;i<this->m_rows;i++)
				{
						for(unsigned int j=0;j<this->m_columns;j++)
						{
							s = Toolbox::doubleToString( (double)getData(i,j,k) );
							fwrite(s.c_str(), s.length(),sizeof(char), pFile);
							if(j+1<this->columns) fputc(',',pFile);
						}
						fputc(';',pFile);
						fputc('\n',pFile);
				}
			}

			fclose(pFile);

        }

        void exportMatrixBinary(const std::string& filename)
		{

			FILE* pFile = fopen(filename.c_str(),"wb");

			//data
			fwrite(this->data, this->m_columns*this->m_rows*this->m_levels,sizeof(T), pFile);

			fclose(pFile);

		}


        enum MATRIX_DATA_ALIGNMENT getDataAlignment() const
        {
        	return this->data_alignment;
        }

        enum SH_DATA_TYPE getDataType() const
        {
        	return this->data_type;
        }


protected:
        int m_rows;
        int m_columns;
        int m_levels;
        T* data;
        bool pinned;

        // reads a csv-token, returns false if terminated by , and true if terminated by ; or EOF
        bool readCSVValue(FILE** pFile, std::string* s)
        {

        	char c[2];
        	c[1] = '\0';
        	*s = ""; //reset string

        	//read chars until ,  ; or EOF found
        	while(1){
        		c[0] = fgetc(*pFile);
        		if(c[0]==',') return false;
        		if(c[0]==';' || c[0]==EOF)
        			{
        			fgetc(*pFile); //read ahead one char, i.e. skip line break. Uncomment this if linebreaks are not being used after ;
        			return true;
        			}
        		#ifdef DEBUG
        		printf("appending %s to %s \n",(char*)&c,(*s).c_str());
        		#endif
        		s->append(c);
        	}

        }

};



}


#endif /* MATRIX_H_ */
