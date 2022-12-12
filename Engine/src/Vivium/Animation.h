#pragma once

#include "Utils.h"
#include "Logger.h"
#include "Vector2.h"
#include "Quad.h"
#include "Texture.h"
#include "TextureAtlas.h"

// TODO: python scripting for animations? or python script to generate timings for animations?
// TODO: better names

namespace Vivium {
	class VIVIUM_API Animation {
	public:
		struct VIVIUM_API Data : Vivium::Streamable {
			std::vector<float> frame_timings; // Holds values for minimum time to be spent on each frame before displaying the next
			std::vector<int> frame_indices;   // Indices of sprites in texture atlas
			std::size_t frame_count = 0;
			
			Data() = default;
			Data(const std::vector<float>& frame_timings);
			Data(const std::vector<float>& frame_timings, const std::vector<int>& frame_indices);
			Data(const std::string& animation_data_file, const Vector2<int>& atlas_dim = NULL);

			void Write(Vivium::Serialiser& s) const override;
			void Read(Vivium::Serialiser& s) override;
		};
	private:
		Animation::Data m_AnimationData;

		std::shared_ptr<TextureAtlas> m_Atlas; // The texture atlas we got the sprites from

		Timer m_Timer;				// Timer to track elapsed time between updates
		float m_FrameTime = 0.0;	// Time spent displaying the current frame
		int m_FrameIndex = 0;		// Index of frame of the animation we're currently displaying

	public:
		static std::string FILE_EXTENSION; // TODO: should deprecate this

		std::shared_ptr<Quad> quad;
		std::shared_ptr<Shader> shader;

		const std::shared_ptr<Texture> GetAtlas() const;

		// NOTE: data_filename refers to the filename of the .animation data file for the texture atlas
		Animation() = default;
		Animation(std::shared_ptr<Quad> quad, std::shared_ptr<Shader> shader, std::shared_ptr<TextureAtlas> atlas, const Animation::Data& animation_data);
		~Animation();

		// Updates the animation, takes current time
		void Update();
	};

	// TODO: prefer this to deprecated Animation and Animation::Data
	// TODO: make animator streamable
	class VIVIUM_API Animator : Streamable {
	public:
		struct VIVIUM_API Data : Streamable {
		public:
			typedef std::pair<float, int> Keyframe_t;
			typedef std::vector<Keyframe_t> KeyframeData_t;

			KeyframeData_t keyframes = {};
			std::shared_ptr<TextureAtlas> atlas = nullptr;

			bool IsValid();

			Data() = default;
			Data(const std::vector<Keyframe_t>& keyframes, const std::shared_ptr<TextureAtlas>& atlas);
			Data(const std::initializer_list<Keyframe_t>& keyframes, const std::shared_ptr<TextureAtlas>& atlas);
			Data(const std::initializer_list<std::pair<float, Vector2<int>>>& keyframes, const std::shared_ptr<TextureAtlas>& atlas);

			Data(const Data& other) = default;

			void Write(Serialiser& s) const override;
			void Read(Serialiser& s) override;
		};

	private:
		Data m_Data;
		Timer m_Timer;

		float m_CurrentKeyframeElapsed = 0.0f; // Time spent displaying current frame
		int m_KeyframeIndex = 0; // Index of keyframe currently being displayed
		bool m_ShouldLoop = false;
		bool m_IsPaused = false;
		bool m_HasEnded = false;

	public:
		void SetShouldLoop(bool should_loop);
		void SetPaused(bool is_paused);

		void Start(int start_frame = 0);
		int Pause(); // Returns frame paused on
		void Resume();

		bool HasEnded() const;

		void Switch(const Data& data);
		void Switch(Data&& data);

		int GetCurrentTextureIndex();

		// TODO: DEBUG ONLY
		inline int DEBUG_GET_KEYFRAMES_SIZE() { return m_Data.keyframes.size(); }

		void Write(Vivium::Serialiser& s) const override;
		void Read(Vivium::Serialiser& s) override;

		// Updates animator
		void Update();

		Animator(const Data& data, bool should_loop = false);
		Animator() = default;
	};
}
