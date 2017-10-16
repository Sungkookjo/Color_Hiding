#include "stdafx.h"
#include "O2Hero.h"
#include "Effect.h"
#include "O2InputManager.h"
#include "O2PassiveObject.h"
#include "O2Item.h"
#include "O2Capsule.h"


O2Hero::O2Hero()
:m_bHide(FALSE)
,m_bHideSound(FALSE)
,m_fAdjustime(0)
,m_fColorDam(1.0f)
,m_fSuperHideTime(0.0f)
,m_bSuperHide(FALSE)
{
	//m_bRendShadow = TRUE;
	m_fSpeed = SPEED_NORMAL;
	m_iState = STATE_IDLE;
	for(int i =0;i<COLOR_MAX;++i){
		m_faColorGage[i]	= 0.0f;
		m_faMaxColGage[i]	= COLOR_GAGE_MAX;
		m_bIsVisible[i]		= FALSE;
	}
}

O2Hero::~O2Hero()
{
	CleanUp();
}

HRESULT O2Hero::CollEffect( O2Object* hit )
{	
	UINT type = hit->GetObjType();
	if((OBJ_TYPE_3D | OBJ_TYPE_PASSIVE) == type ){
		if(_tcscmp(hit->GetName(),_T("GroundArea")) == 0){
			SetState(O2Hero::STATE_DIE);
		}


		if( m_iColor != COLOR_NONE || m_bSuperHide ){
			if(((O23DPassiveObject*)hit)->GetColor() == m_iColor || m_bSuperHide ){
				m_bHide = TRUE;
				m_fAdjustime=2.0f;
			}
		}
	}else if( OBJ_TYPE_ITEM == type ){
		_DEBUG_LOG(_T("Get Item!!\n"));
		((O2Item*)hit)->SetAbility(this);
		O2SOUNDINSTANCE->Play2DEffect(_T("GetItem"),O2SoundManager::S_TYPE_INGAME);
	}else if( OBJ_TYPE_CAPSULE == type ){
		((O2Capsule*)hit)->SetAbility(this);
	}

	if( STATE_JUMP2 == m_iState){
		ChangeState(STATE_JUMP3);
		PlayAnim(ANIM_ONCE);
	}	

	if((m_bHide==TRUE)&&(m_bHideSound==FALSE))
	{
		m_bHideSound=TRUE;
		m_pSoundMgr->Play2DEffect(_T("Hiding"),O2SoundManager::S_TYPE_NAMED,_LoopSound,_T("Hiding"));
	}else if((m_bHide==FALSE)&&(m_bHideSound==TRUE))
	{
		m_bHideSound=FALSE;
		m_pSoundMgr->StopEffect(O2SoundManager::S_TYPE_NAMED,_T("Hiding"));
	}
	
	return S_OK;
}

HRESULT O2Hero::Init( LPDIRECT3DDEVICE9* lpDev ,TCHAR* _filename ,TCHAR* _objname ,D3DXVECTOR3 pPos/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */,D3DXVECTOR3 pSize/*=D3DXVECTOR3(1.0f,1.0f,1.0f) */,D3DXVECTOR3 pRot/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */ )
{
	// 부모의 초기화 실행
	if(FAILED(O23DActiveObject::Init(lpDev,_filename,_objname,pPos,pSize,pRot)))
		return E_FAIL;

	// 임시
	m_faColorGage[COLOR_R] = COLOR_GAGE_MAX;
	m_faColorGage[COLOR_G] = COLOR_GAGE_MAX;
	m_faColorGage[COLOR_B] = COLOR_GAGE_MAX;
	m_faColorGage[COLOR_P] = COLOR_GAGE_MAX;
	m_bIsVisible[COLOR_R]	=	FALSE;
	m_bIsVisible[COLOR_G]	=	TRUE;
	m_bIsVisible[COLOR_B]	=	FALSE;
	m_bIsVisible[COLOR_P]	=	FALSE;

	m_adwAniId[STATE_IDLE] = GetAniIndex( "stand" );
	m_adwAniId[STATE_WALK] = GetAniIndex( "walk" );
	m_adwAniId[STATE_BACK] = GetAniIndex( "reverse" );
	m_adwAniId[STATE_RUN ] = GetAniIndex( "run" );
	m_adwAniId[STATE_JUMP1] = GetAniIndex( "jump1" );
	m_adwAniId[STATE_JUMP2] = GetAniIndex( "jump2" );
	m_adwAniId[STATE_JUMP3] = GetAniIndex( "jump3" );
	m_adwAniId[STATE_DIE] = GetAniIndex( "die" );
	m_adwAniId[STATE_DANCE] = GetAniIndex( "dance" );
	m_adwAniId[STATE_CLEAR] = GetAniIndex( "clear" );

	for(int i =0;i<STATE_MAX;++i){
		if(ANIMINDEX_FAIL == m_adwAniId[i]){
			return E_FAIL;
		}
	}

	m_iState = STATE_IDLE;
	m_vDestPos = m_vPos;
	UpDateAnim();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 키입력 처리
void O2Hero::ProcInput( O2InputManager* pInput )
{
	static float time=0.0f;
	UINT state = STATE_IDLE;

	time += (*m_fpDelta);

	// 색 변경
	if(pInput->IsKeyPress(DIK_1)){
		if(m_bIsVisible[COLOR_G])
			SetColor(COLOR_G);
	}else	if(pInput->IsKeyPress(DIK_2)){
		if(m_bIsVisible[COLOR_R])
			SetColor(COLOR_R);
	}else	if(pInput->IsKeyPress(DIK_3)){
		if(m_bIsVisible[COLOR_B])
			SetColor(COLOR_B);
	}else	if(pInput->IsKeyPress(DIK_4)){
		if(m_bIsVisible[COLOR_P])
			SetColor(COLOR_P);
	}else	if(pInput->IsKeyPress(DIK_5)){
		//SetColor(COLOR_NONE);
	}else	if(pInput->IsKeyPress(DIK_GRAVE)){
		SetColor(COLOR_NONE);
	}

	// 임시
	if(pInput->IsKeyPress(DIK_F1)){
		SetColorVisible(COLOR_G,!m_bIsVisible[COLOR_G]);
	}else	if(pInput->IsKeyPress(DIK_F2)){
		SetColorVisible(COLOR_R,!m_bIsVisible[COLOR_R]);
	}else	if(pInput->IsKeyPress(DIK_F3)){
		SetColorVisible(COLOR_B,!m_bIsVisible[COLOR_B]);
	}else	if(pInput->IsKeyPress(DIK_F4)){
		SetColorVisible(COLOR_P,!m_bIsVisible[COLOR_P]);
	}else	if(pInput->IsKeyPress(DIK_F5)){
	}
	if(pInput->IsKeyDown(DIK_F)){
		SetColorGage(COLOR_R,GetColorMaxGage(COLOR_R));
		SetColorGage(COLOR_G,GetColorMaxGage(COLOR_B));
		SetColorGage(COLOR_B,GetColorMaxGage(COLOR_B));
		SetColorGage(COLOR_P,GetColorMaxGage(COLOR_P));
	}


	// 캐릭터 이동
	if(pInput->IsKeyDown(DIK_A)){
		state = STATE_WALK;
		MoveLeft();
	}else if(pInput->IsKeyDown(DIK_D)){
		state = STATE_WALK;
		MoveRight();
	}

	if(pInput->IsKeyDown(DIK_S)){
		state = STATE_BACK;
		MoveBack();
	}else if(pInput->IsKeyPress(DIK_W)){
		if( time <= 0.5f){
			state = STATE_RUN;
			time = 0.0f;
		}else{
			state = STATE_WALK;
			time = 0.0f;
		}
	}else if(pInput->IsKeyDown(DIK_W)){
		if(m_iState == STATE_RUN){
			state = STATE_RUN;
			MoveFront(2.0f);
		}else{
			state = STATE_WALK;
			MoveFront();
		}
	}

	if(pInput->IsKeyPress(DIK_E)){
		state = STATE_DANCE;
		if(m_iState < STATE_DANCE){
			PlayAnim(ANIM_ONCE,FALSE);
		}
	}

	// 캐릭터 점프
	if(pInput->IsKeyPress(DIK_SPACE)){
		if(m_iState < STATE_JUMP1){
			if( STATE_RUN == m_iState )
				MoveUp(1.5f);
			else
				MoveUp();
			state = STATE_JUMP1;
			PlayAnim(ANIM_ONCE,FALSE);
		}
	}

	// 캐릭터 점프
	if(pInput->IsKeyPress(DIK_N)){
		state = STATE_CLEAR;
	}else if(pInput->IsKeyPress(DIK_B)){
		state = STATE_DIE;
	}

	DIMOUSESTATE MouseState = pInput->GetMouseState();
	if(!(MouseState.rgbButtons[0] || MouseState.rgbButtons[1])){
		RotY(MouseState.lX *0.1f);
	}

	// 이전 캐릭터 위치와 현재 위치를 비교하여 
	// 낙하 애니메이션으로 바꿔줌
	// + 는 float 앱실론 값
	if(m_vPos.y + 0.3f < m_vDestPos.y){
		ChangeState(STATE_JUMP2);
		PlayAnim(ANIM_ONCE,FALSE);
	}else{
		ChangeState(state);
	}

	m_vDestPos = m_vPos;

	if(m_iColor != COLOR_NONE){
		m_faColorGage[m_iColor] -= m_fColorDam * (*m_fpDelta);
		if(m_faColorGage[m_iColor] <= 0.0f){
			SetColor(COLOR_NONE);
		}
	}
}

void O2Hero::UpDateAnim()
{
	DWORD dwNewTrack = ( m_dwCurTrack == 0 ? 1 : 0 );
	LPD3DXANIMATIONSET pAS;

	// 새로 설정할 애니셋 얻어옴
	m_pAnimController->GetAnimationSet( m_adwAniId[m_iState], &pAS );

	// 다른 트랙에 애니셋 설정
	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAS );
	// 현재 애니의 길이를 받아온다.
	m_lfAniTime = pAS->GetPeriod();
	// 현재 시간 저장
	m_lfDestTime = m_fCurTime;
	pAS->Release();

	// 트랙의 모든 이벤트 해제
	m_pAnimController->UnkeyAllTrackEvents( m_dwCurTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	// 트랙의 속도, 가중치 설정
	m_pAnimController->KeyTrackEnable( m_dwCurTrack, FALSE, (m_fPreTime) + IDLE_TRANSITION_TIME );
	m_pAnimController->KeyTrackSpeed( m_dwCurTrack, 0.0f, (m_fPreTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_dwCurTrack, 0.0f, (m_fPreTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	//m_fCurTime = 0.0f;
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, (m_fCurTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, (m_fCurTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackPosition( dwNewTrack, 0.0 );

	m_dwCurTrack = dwNewTrack;
}

VOID O2Hero::ChangeState( UINT pState )
{
	if(m_iState != pState){
		// 바꾸려는 상태보다 현재 상태가 상위 값이면
		if( m_iState > pState ){
			if(pState != STATE_IDLE){
				if( m_iState < STATE_JUMP1 ){
					m_iState = pState;
					UpDateAnim();
				}
			}else{
				if(STATE_DANCE > m_iState){
					m_iState = pState;
					UpDateAnim();
				}
			}
			// 애니가 멈춰 있을경우 재생
			if(m_bIsAniStop){
				if(m_iState != STATE_JUMP2){
					m_iState = pState;
					UpDateAnim();
					PlayAnim();
				}else{
					PlayAnim(ANIM_ONCE);
				}
			}
		}else{
			m_iState = pState;
			UpDateAnim();
		}
	}
}

HRESULT O2Hero::UpDate()
{
	if(m_bSuperHide){
		m_fSuperHideTime += (*m_fpDelta);
		if( m_fSuperHideTime > TIME_SUPERHIDE ){
			m_bSuperHide = FALSE;
			m_bReflectShadow = m_bDestReflectShadow;
			m_bLineRend = m_bDestRendLine;
			m_bRendShadow = m_bDestRendShadow;
			m_bUseCellEffect = m_bDestUseLight;
		}
	}
	UpdateNxtoDx();
	NxVec3 tempfoce;
	m_fPreTime = m_fCurTime;
	// 애니메이션 프레임 움직임
	if( m_pAnimController != NULL ){
		if( STATE_DANCE < m_iState){
			// 공중에 떠있는 모션일때 2배 재생
			if( STATE_JUMP2 == m_iState){
				m_pAnimController->AdvanceTime( (*m_fpDelta)*2.0f, NULL );
				tempfoce = m_pActor->getLinearVelocity();
				// 현재 y축 속도가 일정 수치보다 낮을 경우 
				if(tempfoce.y <= 1.01f){
					// 떨어지기 시작할 때
					// 최고점에서 오래 머무는것 방지 하기 위해 일정 속도 이하로 떨어지면 바로 중력으로 힘 변환
					if( tempfoce.y >= -9.0f ){
						SetForce(0.0f,-20.8f,0.0f);
					}else{
						// 수정부분08221727
						// 떨어지고 있을때
						AddForce(0.0f,-20.8f*1.5f*(*m_fpDelta),0.0f);
					}
				}else{
					// 올라가고 있는 경우
					AddForce(0.0f,-9.8f*1.5f*(*m_fpDelta),0.0f);
				}
				m_fCurTime += (*m_fpDelta)*2.0f;			
			}else if( STATE_JUMP1 == m_iState || STATE_JUMP3 == m_iState){
				// 점프 도약 모션 3배 재생
				m_pAnimController->AdvanceTime( (*m_fpDelta)*3.0f, NULL );
				m_fCurTime += (*m_fpDelta)*3.0f;
			}else{
				// 나머지는 그냥 재생
				m_pAnimController->AdvanceTime( (*m_fpDelta), NULL );
				m_fCurTime += (*m_fpDelta);
			}
		}else{
			// 나머지는 그냥 재생
			m_pAnimController->AdvanceTime( (*m_fpDelta), NULL );
			m_fCurTime += (*m_fpDelta);
		}
	}

	// 애니 once 검사
	if( ANIM_LOOP != m_iCount ){
		// 애니가 멈춰야될 시간과 현재 시간 체크
		double lerp = m_fCurTime - m_lfDestTime;
		if( m_lfAniTime <=  lerp){
			// 카운트 증가
			++m_iCurCount;
			// 애니 검사 시간을 현재 시간 - 오바 시간
			m_lfDestTime = m_fCurTime - (lerp - m_lfAniTime);
			if( m_iCurCount >= m_iCount ){
				StopAnim();
			}
		}
	}

	// 애니가 정지 했을때 정지한 애니메이션에 따라 다음 애니로 change
	if(m_bIsAniStop){
		if(m_iState == STATE_JUMP1){
			ChangeState(STATE_JUMP2);
			PlayAnim(ANIM_ONCE);
		}else if(m_iState == STATE_JUMP3 || m_iState == STATE_DANCE){
			ChangeState(STATE_IDLE);			
		}
		if(m_iState != STATE_JUMP2){
			PlayAnim();
		}
	}

	// 임시
	m_fAdjustime-=(*m_fpDelta);

	return S_OK;
}

void O2Hero::ProcInputHelp( O2InputManager* pInput )
{
	static float time=0.0f;
	UINT state = STATE_IDLE;

	time += (*m_fpDelta);

	// 색 변경
	if(pInput->IsKeyPress(DIK_1)){
		SetColor(COLOR_G);
	}else	if(pInput->IsKeyPress(DIK_2)){
		SetColor(COLOR_R);
	}else	if(pInput->IsKeyPress(DIK_3)){
		SetColor(COLOR_B);
	}else	if(pInput->IsKeyPress(DIK_4)){
		SetColor(COLOR_P);
	}else	if(pInput->IsKeyPress(DIK_5)){
		//SetColor(COLOR_NONE);
	}else	if(pInput->IsKeyPress(DIK_GRAVE)){
		SetColor(COLOR_NONE);
	}

	// 캐릭터 이동
	if(pInput->IsKeyDown(DIK_A)){
		state = STATE_WALK;
	}else if(pInput->IsKeyDown(DIK_D)){
		state = STATE_WALK;
	}

	if(pInput->IsKeyDown(DIK_S)){
		state = STATE_BACK;
	}else if(pInput->IsKeyPress(DIK_W)){
		if( time <= 0.5f){
			state = STATE_RUN;
			time = 0.0f;
		}else{
			state = STATE_WALK;
			time = 0.0f;
		}
	}else if(pInput->IsKeyDown(DIK_W)){
		if(m_iState == STATE_RUN){
			state = STATE_RUN;
		}else{
			state = STATE_WALK;
		}
	}

	if(pInput->IsKeyPress(DIK_E)){
		state = STATE_DANCE;
		if( m_iState < STATE_DANCE ){
			PlayAnim(ANIM_ONCE,FALSE);
		}
	}

	// 캐릭터 점프
	if(pInput->IsKeyPress(DIK_SPACE)){
		if(m_iState < STATE_JUMP1){
			state = STATE_JUMP1;
			PlayAnim(ANIM_ONCE,TRUE);
		}
	}
	
	RotY(10.0f * (*m_fpDelta));

	// 이전 캐릭터 위치와 현재 위치를 비교하여 
	// 낙하 애니메이션으로 바꿔줌
	// +0.1f는 float 앱실론 값
	if( STATE_JUMP2 == m_iState ){
		if(m_bIsAniStop){
			ChangeState(STATE_JUMP3);
			PlayAnim(ANIM_ONCE);
		}
	}else{
		ChangeState(state);
	}

	m_vDestPos = m_vPos;

}

VOID O2Hero::SetColorVisible( UINT pColor,BOOL pvisible /*= TRUE*/ )
{
	m_bIsVisible[pColor] = pvisible;
	if( pColor == m_iColor && FALSE == pvisible ){
		SetColor(COLOR_NONE);
	}
}

VOID O2Hero::SetFullColor()
{
	for(int i =0;i<COLOR_MAX;++i){
		m_faColorGage[i] = m_faMaxColGage[i];
	}
}

VOID O2Hero::SetMaxColor( float fcolor )
{
	for(int i =0;i<COLOR_MAX;++i){
		m_faMaxColGage[i] = fcolor;
	}
}

VOID O2Hero::SetSuperHide()
{
	if(!m_bSuperHide){
		m_bSuperHide = TRUE;
		m_fSuperHideTime = 0.0f;
		m_bDestReflectShadow = m_bReflectShadow;
		m_bDestRendLine = m_bLineRend;
		m_bDestRendShadow = m_bRendShadow;
		m_bDestUseLight = m_bUseCellEffect;
		m_bUseCellEffect = FALSE;
	//	m_bLineRend = FALSE;
		m_bRendShadow = FALSE;
		m_bDestReflectShadow = TRUE;
	}else{
		m_fSuperHideTime = 0.0f;
	}
}