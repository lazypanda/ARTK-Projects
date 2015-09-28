//*******************
// ARToolKitÉâÉbÉpÅ[
//*******************

#ifndef		_ARTK_H_
#define		_ARTK_H_

#include	<AR/config.h>
#include	<AR/video.h>
#include	<AR/param.h>
#include	<AR/ar.h>
#include	<AR/gsub_lite.h>
#include	"nyar_NyARTransMat.h"

#include	"../MMD/VecMatQuat.h"


enum
{
	ARMK_BASE = 0,
	ARMK_CURSOR,

	ARMK_GOO,
	ARMK_CHOKI,
	ARMK_PAR,

	ARMK_MAXNUM
};

class cARTK
{
	private :
		ARParam		m_sCameraParam;

		ARGL_CONTEXT_SETTINGS_REF
					m_pArglSettings;
		double		m_dViewScaleFactor;

		nyar_NyARTransMat_O2_t
					*m_pNyARInst;

		double		m_dmatRotX[16];
		Matrix		m_matRotX;

		ARUint8		*m_pARTImage;

		int			m_iThreshold;
		bool		m_bMarkersFound;

		struct MARKER_INFO
		{
			int		iPattID;
			bool	bVisible;
			double	dWidth;
			double	dCenterPos[2];
			double	dTransMat[3][4];
		};

		MARKER_INFO	m_sMarkerInfo[ARMK_MAXNUM];

	public :
		cARTK( void );
		~cARTK( void );

		bool initialize( void );
		bool initArgl( void );

		bool update( void );
		void display( void );

		void getCameraPos( Vector3 *pvec3CamPos, int iPattIndex = 0 );
		void getMarkerPos( Vector3 *pvec3MarkerPos, int iPattIndexBase, int iPattIndexTarget );

		void getProjectionMat( double dMat[16] );
		void getModelViewMat( double dMat[16], int iPattIndex );
		void addViewScaleFactor( double dAdd );

		void cleanup( void );

		inline void addThreshold( int iValue )
		{
			m_iThreshold += iValue;
			if( m_iThreshold < 80 )		m_iThreshold = 80;
			if( 200 < m_iThreshold )	m_iThreshold = 200;
		}
		inline int getThreshold( void ){ return m_iThreshold; }

		inline bool isMarkersFound( void ){ return m_bMarkersFound; }
		inline bool isMarkerVisible( int iPattIndex ){ return m_sMarkerInfo[iPattIndex].bVisible; }
};

#endif	// _ARTK_H_
