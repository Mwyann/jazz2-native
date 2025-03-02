#pragma once

#include <ctime>
#include <cstdlib>
#include "../Primitives/Rect.h"
#include "SceneNode.h"
#include "ParticleAffectors.h"
#include "BaseSprite.h"

namespace nCine
{
	class Texture;
	class Particle;
	struct ParticleInitializer;

	/// Represents a particle system
	class ParticleSystem : public SceneNode
	{
	public:
		/// Constructs a particle system with the specified maximum amount of particles
		ParticleSystem(SceneNode* parent, std::uint32_t count, Texture* texture);
		/// Constructs a particle system with the specified maximum amount of particles and the specified texture rectangle
		ParticleSystem(SceneNode* parent, std::uint32_t count, Texture* texture, Recti texRect);
		~ParticleSystem() override;

		ParticleSystem& operator=(const ParticleSystem&) = delete;
		ParticleSystem(ParticleSystem&&);
		ParticleSystem& operator=(ParticleSystem&&);

		/// Returns a copy of this object
		inline ParticleSystem clone() const {
			return ParticleSystem(*this);
		}

		/// Adds a particle affector
		inline void addAffector(std::unique_ptr<ParticleAffector> affector) {
			affectors_.push_back(affector.release());
		}
		/// Deletes all particle affectors
		void clearAffectors();
		/// Emits particles with the specified initialization parameters
		void emitParticles(const ParticleInitializer& init);
		/// Kills all alive particles
		void killParticles();

		/// Returns the local space flag of the system
		inline bool inLocalSpace() const {
			return inLocalSpace_;
		}
		/// Sets the local space flag of the system
		inline void setInLocalSpace(bool inLocalSpace) {
			inLocalSpace_ = inLocalSpace;
		}

		/// Returns the total number of particles in the system
		inline std::uint32_t numParticles() const {
			return std::uint32_t(particleArray_.size());
		}
		/// Returns the number of particles currently alive
		inline std::uint32_t numAliveParticles() const {
			return std::uint32_t(particleArray_.size()) - poolTop_ - 1;
		}

		/// Sets the texture object for every particle
		void setTexture(Texture* texture);
		/// Sets the texture source rectangle for every particle
		void setTexRect(const Recti& rect);

		/// Sets the transformation anchor point for every particle
		void setAnchorPoint(float xx, float yy);
		/// Sets the transformation anchor point for every particle with a `Vector2f`
		void setAnchorPoint(Vector2f point);

		/// Flips the texture rect horizontally for every particle
		void setFlippedX(bool flippedX);
		/// Flips the texture rect vertically for every particle
		void setFlippedY(bool flippedY);

		/// Sets the blending factors preset for every particle
		void setBlendingPreset(DrawableNode::BlendingPreset blendingPreset);
		/// Sets the source and destination blending factors for every particle
		void setBlendingFactors(DrawableNode::BlendingFactor srcBlendingFactor, DrawableNode::BlendingFactor destBlendingFactor);

		/// Sets the rendering layer for every particle
		void setLayer(uint16_t layer);

		void OnUpdate(float timeMult) override;

		inline static ObjectType sType() {
			return ObjectType::ParticleSystem;
		}

	protected:
		/// Protected copy constructor used to clone objects
		ParticleSystem(const ParticleSystem& other);

	private:
		/// Particle pool size
		std::uint32_t poolSize_;
		/// Index of the next free particle in the pool
		std::int32_t poolTop_;
		/// Pool containing available particles (only dead ones)
		SmallVector<Particle*, 0> particlePool_;
		/// Array containing every particle (dead or alive)
		SmallVector<Particle*, 0> particleArray_;

		/// Array of particle affectors
		SmallVector<ParticleAffector*, 0> affectors_;

		/// Flag indicating whether the system should be simulated in local space
		bool inLocalSpace_;
	};

}
