//****************
// ‚¶‚á‚ñ‚¯‚ñ‚ÌŽè
//****************

#ifndef		_JANKEN_HAND_H_
#define		_JANKEN_HAND_H_

#define		JANKEN_GOO		(1 << 0)
#define		JANKEN_CHOKI	(1 << 1)
#define		JANKEN_PAR		(1 << 2)

class cJankenHand
{
	private :
		unsigned int	m_uiGooTexID,
						m_uiChokiTexID,
						m_uiParTexID;

	public :
		cJankenHand( void );
		~cJankenHand( void );

		bool initialize( void );

		void render( unsigned int uiJankenType );

		void release( void );

};

#endif	// _JANKEN_HAND_H_
