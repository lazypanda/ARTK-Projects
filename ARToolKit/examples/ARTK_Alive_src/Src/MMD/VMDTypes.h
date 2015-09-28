//*******************************
// VMD���[�V�����p�e��\���̒�`
//*******************************

// ���̍\���̒�`�́AMMD�̃��f���f�[�^�ɂ��Č��X���v(http://jbbs.livedoor.jp/bbs/read.cgi/music/23040/1219738115/)��
// �x����������(http://yumin3123.at.webry.info/)��VMDConverter�̃\�[�X�t�@�C�����Q�l�ɂ����Ă��������܂���

#ifndef	_VMDTYPES_H_
#define	_VMDTYPES_H_

#include	"VecMatQuat.h"

#pragma pack( push, 1 )

// �t�@�C���w�b�_
struct VMD_Header
{
	char	szHeader[30];			// "Vocaloid Motion Data 0002"
	char	szModelName[20];		// �Ώۃ��f����
};

// ���[�V�����f�[�^
struct VMD_Motion
{
	char	szBoneName[15];			// �{�[����

	unsigned long	ulFrameNo;		// �t���[���ԍ�

	Vector3	vec3Position;			// �ʒu
	Vector4	vec4Rotate;				// ��](�N�H�[�^�j�I��)

	char	cInterpolation1[16];	// ��ԏ��
	char	cInterpolation2[16];
	char	cInterpolation3[16];
	char	cInterpolation4[16];
};

// �\��f�[�^
struct VMD_Face
{
	char	szFaceName[15];		// �\�

	unsigned long	ulFrameNo;	// �t���[���ԍ�

	float	fFactor;			// �u�����h��
};

#pragma pack( pop )

#endif	// _VMDTYPES_H_