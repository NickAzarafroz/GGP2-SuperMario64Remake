#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//TODO_W6_()

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		if (passedTicks > m_CurrentClip.duration) 
		{
			passedTicks = fmodf(passedTicks, m_CurrentClip.duration);
		}
		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;

			if(m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if(m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}


		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (size_t i{}; i < m_CurrentClip.keys.size(); ++i)
		{
			const auto& key{ m_CurrentClip.keys[i] };

			if (key.tick > m_TickCount)
			{
				keyB = key;
				keyA = m_CurrentClip.keys[i - 1];
				break;
			}
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		const float blendFactor{ (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick) };
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector

		for (size_t i{}; i < m_pMeshFilter->m_BoneCount; i++)
		{
			const auto& transformA = keyA.boneTransforms[i];
			const auto& transformB = keyB.boneTransforms[i];

			XMMATRIX transformMatrixA = XMLoadFloat4x4(&transformA);
			XMMATRIX transformMatrixB = XMLoadFloat4x4(&transformB);

			XMVECTOR posA{}, posB{}, rotA{}, rotB{}, scaleA{}, scaleB{};
			DirectX::XMMatrixDecompose(&scaleA, &rotA, &posA, transformMatrixA);
			DirectX::XMMatrixDecompose(&scaleB, &rotB, &posB, transformMatrixB);

			XMVECTOR pos = XMVectorLerp(posA, posB, blendFactor);
			XMVECTOR rot = XMQuaternionSlerp(rotA, rotB, blendFactor);
			XMVECTOR scale = XMVectorLerp(scaleA, scaleB, blendFactor);

			const XMMATRIX boneTransform{ XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rot) * XMMatrixTranslationFromVector(pos) };

			XMFLOAT4X4 newBoneTransform{};
			XMStoreFloat4x4(&newBoneTransform, boneTransform);

			m_Transforms.push_back(newBoneTransform);
			
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for(const auto& animationClip : m_pMeshFilter->m_AnimationClips)
	{
		if(animationClip.name == clipName)
		{
			SetAnimation(animationClip);
			return;
		}
	}

	Reset();
	Logger::LogWarning(L"No animation clip with the given name was found!");
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if(clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		const auto& animationClip = m_pMeshFilter->m_AnimationClips[clipNumber];
		SetAnimation(animationClip);
	}
	else
	{
		Reset();
		Logger::LogWarning(L"Clip number is out of range!");
		return;
	}
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	TODO_W6_()
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//TODO_W6_()
	//If pause is true, set m_IsPlaying to false
	if(pause)
	{
		m_IsPlaying = false;
	}
	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0.f;
	m_AnimationSpeed = 1.f;
	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	if(m_ClipSet)
	{
		const auto& boneTransform = m_CurrentClip.keys[0].boneTransforms;
		m_Transforms.assign(boneTransform.begin(), boneTransform.end());
	}
	else
	{
		XMFLOAT4X4 identityMatrix{};
		identityMatrix._11 = 1.0f;
		identityMatrix._22 = 1.0f;
		identityMatrix._33 = 1.0f;
		identityMatrix._44 = 1.0f;

		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identityMatrix);
	}
}
