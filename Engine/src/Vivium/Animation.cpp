#include "Animation.h"
namespace Vivium {
	std::string Animation::FILE_EXTENSION = "";

	Animation::Data::Data(const std::vector<float>& frame_timings)
	{
		VIVIUM_SCOPE;

		int sprite_index = 0;

		// Iterate timing data passed
		for (float frame_time : frame_timings) {
			// If its a valid time value
			if (frame_time > 0.0) {
				this->frame_timings.push_back(frame_time);
				++frame_count;
				frame_indices.push_back(sprite_index);
			}

			++sprite_index;
		}
	}

	Animation::Data::Data(const std::vector<float>& frame_timings, const std::vector<int>& frame_indices)
		: frame_timings(frame_timings), frame_indices(frame_indices), frame_count(frame_indices.size())
	{}

	Animation::Data::Data(const std::string& animation_data_file, const Vector2<int>& atlas_dim)
	{
		VIVIUM_SCOPE;

		// Load text
		std::string* filetext = Utils::ReadFile(Texture::PATH + animation_data_file + Animation::FILE_EXTENSION);

		// Split by comma
		std::vector<std::string> parts = Utils::SplitString(*filetext, ",");

		int loop_start = 0;
		int sprite_index = 0;
		bool indices_list = false;

		// Check if the first part of the data is a #n, indicating to skip n sprites forward
		if (parts[0][0] == '#') {
			// Erase first character (which is #)
			parts[0].erase(parts[0].begin());
			// Set our starting index to that value
			sprite_index = std::stoi(parts[0]);
			// Start at next index since the first isn't a float value
			loop_start = 1;
		}

		// Indicating we are entering a list of indices
		if (parts[0][0] == '{') {
			indices_list = true;
			
			LogTrace("Detected indices list");
			
			// TODO: So unreadable
			for (int i = 0; i < parts.size(); i += 2) {
				std::string index_str = parts[i] + "," + parts[i + 1];
				// TODO: use regex next time
				Utils::EraseSubstring(index_str, "{");
				Utils::EraseSubstring(index_str, "}\n");

				if (index_str[0] != '(') break;

				Utils::EraseSubstring(index_str, "(");
				Utils::EraseSubstring(index_str, ")");
				
				std::vector<std::string> index_split = Utils::SplitString(index_str, ",");
				int x_index = std::stoi(index_split[0]);
				int y_index = std::stoi(index_split[1]);
				int index_1d = x_index + y_index * atlas_dim.x;

				frame_indices.push_back(index_1d);
			}

			frame_count = frame_indices.size();
			loop_start = frame_indices.size() * 2;
		}

		// Iterate over each string
		for (int i = loop_start; i < parts.size(); i++) {
			float value = 0.0;

			try {
				// Convert value to float
				value = std::stof(parts[i]);
			}
			// If animation file contains value that cannot be converted to float
			catch (std::invalid_argument) {
				LogWarn("Invalid value in {}.animation: {}", animation_data_file, parts[i]);
			}

			// NOTE: Sometimes animations are included in larger texture atlases/multiple animations are in the same texture atlas,
			//	so we can enter lots of 0s at the start of our animation.data file so we start at the first frame of our animation

			// If value is more than 0, add the timing to our timings
			if (value > 0.0) {
				frame_timings.push_back(value);
				++frame_count;
				if (!indices_list) { frame_indices.push_back(sprite_index); }
			}
			
			++sprite_index;
		}

		delete filetext;
	}

	void Animation::Data::Write(Vivium::Serialiser& s) const
	{
		s.Write(frame_timings);
		s.Write(frame_indices);
		s.Write(frame_count);
	}

	void Animation::Data::Read(Vivium::Serialiser& s)
	{
		s.Read(&frame_timings);
		s.Read(&frame_indices);
		s.Read(&frame_count);
	}

	const std::shared_ptr<Texture> Animation::GetAtlas() const
	{
		return m_Atlas->GetAtlas();
	}

	Animation::Animation(std::shared_ptr<Quad> quad, std::shared_ptr<Shader> shader, std::shared_ptr<TextureAtlas> atlas, const Animation::Data& animation_data)
		: quad(quad), shader(shader), m_Atlas(atlas), m_AnimationData(animation_data)
	{}

	Animation::~Animation() {}

	void Animation::Update()
	{
		VIVIUM_SCOPE;

		// Get elapsed time
		float elapsed = m_Timer.GetElapsed();

		// Add delta time to time spent on current frame
		m_FrameTime += elapsed;

		// Get time that is meant to be spent on this frame
		float this_frame_timing = m_AnimationData.frame_timings[m_FrameIndex];

		// If we have spent enough time on this frame
		if (m_FrameTime > this_frame_timing) {
			// Increment frame counter
			++m_FrameIndex;
			m_FrameIndex %= m_AnimationData.frame_count;
			// Reset current frame time
			m_FrameTime -= this_frame_timing;
			// Change texture coordinates
			m_Atlas->Set(quad.get(), m_AnimationData.frame_indices[m_FrameIndex]);
		}
	}

	bool Animator::Data::IsValid()
	{
		if (keyframes.size() == 0) {
			LogWarn("0 frame animation found!");

			return false;
		}
		else {
			bool found_real_keyframe = false;

			for (const Keyframe_t& keyframe : keyframes) {
				if (keyframe.first != 0.0f) {
					found_real_keyframe = true;
					
					break;
				}
			}

			if (!found_real_keyframe) {
				LogWarn("No keyframe with non-zero timing found in animation!");

				return false;
			}
		}

		return true;
	}

	Animator::Data::Data(const std::vector<Keyframe_t>& keyframes, const std::shared_ptr<TextureAtlas>& atlas)
		: keyframes(keyframes), atlas(atlas)
	{}

	Animator::Data::Data(const std::initializer_list<Keyframe_t>& keyframes, const std::shared_ptr<TextureAtlas>& atlas)
		: keyframes(keyframes), atlas(atlas)
	{}

	Animator::Data::Data(const std::initializer_list<std::pair<float, Vector2<int>>>& _keyframes, const std::shared_ptr<TextureAtlas>& atlas)
		: atlas(atlas)
	{
		keyframes.reserve(_keyframes.size());

		for (auto& pair : _keyframes) {
			keyframes.emplace_back(pair.first, atlas->GetIndex(pair.second));
		}
	}

	void Animator::Data::Write(Serialiser& s) const
	{
		s.Write(keyframes);
	}
	
	void Animator::Data::Read(Serialiser& s)
	{
		s.Read(&keyframes);
	}

	void Animator::SetShouldLoop(bool should_loop) 
	{
		m_ShouldLoop = should_loop;
		m_HasEnded = false;
	}

	void Animator::SetPaused(bool is_paused) 
	{
		m_IsPaused = is_paused;
	}

	void Animator::Start(int start_frame)
	{
		m_KeyframeIndex = start_frame;
		m_CurrentKeyframeElapsed = 0.0f;
		m_HasEnded = false;
	}

	int Animator::Pause()
	{
		m_IsPaused = true;

		return m_KeyframeIndex;
	}

	void Animator::Resume()
	{
		m_IsPaused = false;
	}

	bool Animator::HasEnded() const {
		return m_HasEnded;
	}

	void Animator::Switch(const Data& data) 
	{
		m_Data = data;
		m_KeyframeIndex = 0;
		m_HasEnded = false;
		m_CurrentKeyframeElapsed = 0.0f;

		if (!m_Data.IsValid()) {
			LogFatal("Invalid animation data passed to animator!");
		}
	}

	void Animator::Switch(Data&& data)
	{
		m_Data = std::move(data);
		m_KeyframeIndex = 0;
		m_HasEnded = false;
		m_CurrentKeyframeElapsed = 0.0f;

		if (!m_Data.IsValid()) {
			LogFatal("Invalid animation data passed to animator!");
		}
	}

	int Animator::GetCurrentTextureIndex()
	{
		if (m_Data.keyframes.size() == 0) {
			return -1;
		}

		return m_Data.keyframes[m_KeyframeIndex].second;
	}

	void Animator::Write(Vivium::Serialiser& s) const {
		s.Write(m_Data);
		s.Write(m_Timer);
		s.Write(m_CurrentKeyframeElapsed);
		s.Write(m_KeyframeIndex);
		s.Write(m_ShouldLoop);
		s.Write(m_IsPaused);
	}

	void Animator::Read(Vivium::Serialiser& s) {
		s.Read(&m_Data);
		s.Read(&m_Timer);
		s.Read(&m_CurrentKeyframeElapsed);
		s.Read(&m_KeyframeIndex);
		s.Read(&m_ShouldLoop);
		s.Read(&m_IsPaused);
	}

	void Animator::Update()
	{
		// If we have some invalid data
		if (m_Data.keyframes.size() == 0 || m_HasEnded) {
			return;
		}

		if (m_IsPaused) {
			m_Timer.Reset(); // TODO: ideally you could pause a timer?
			
			return;
		}

		// Get elapsed time
		float elapsed = m_Timer.GetElapsed();

		// Add delta time to time spent on current frame
		m_CurrentKeyframeElapsed += elapsed;

		// Get current keyframe
		Data::Keyframe_t& this_keyframe = m_Data.keyframes[m_KeyframeIndex];

		// If we have spent enough time on this frame
		while (m_CurrentKeyframeElapsed > this_keyframe.first) {
			// If we're on the last frame
			if (m_KeyframeIndex == m_Data.keyframes.size() - 1) {
				// If we're looping and on the last frame, reset to first frame
				if (m_ShouldLoop) {
					m_KeyframeIndex = 0;
				}
				// Otherwise, stay on this frame since the animation has ended
				else {
					m_HasEnded = true;

					// Also break the loop since nothing else needs to be done
					break;
				}
			}
			else {
				++m_KeyframeIndex;
			}

			// Turn back elapsed time
			m_CurrentKeyframeElapsed -= this_keyframe.first;

			// So we can skip multiple frames in one update
			this_keyframe = m_Data.keyframes[m_KeyframeIndex];
		}
	}

	Animator::Animator(const Data& data, bool should_loop)
		: m_Data(data), m_ShouldLoop(should_loop)
	{
		if (!m_Data.IsValid()) {
			LogFatal("Invalid animation data passed to animator!");
		}
	}
}