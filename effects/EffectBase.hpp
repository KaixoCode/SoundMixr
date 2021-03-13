#pragma once
#include <nlohmann/json.hpp>

#define DLLDIR  __declspec(dllexport)   // export DLL information

#define constrain(x, y, z) (x < y ? y : x > z ? z : x)

namespace Effects
{
	/**
	 * Simple pair used for size/position/range.
	 */
	template<typename T>
	class Pair
	{
	public:
		union { T x, start, r, width; };
		union { T y, end, g, height; };
	};

	/**
	 * Parameter types.
	 */
	enum class ParameterType
	{
		Knob, Slider, VolumeSlider
	};

	/**
	 * Basis for any Effect related object
	 */
	class Object
	{
	public:

		/**
		 * Set the size.
		 * @param size size
		 */
		virtual void Size(const Pair<int>& size) { m_Size = size; }

		/**
		 * Get the size.
		 * @return size
		 */
		virtual auto Size() -> Pair<int> { return m_Size; }

		/**
		 * Set the position.
		 * @param p position
		 */
		virtual void Position(const Pair<int>& p) { m_Position = p; }

		/**
		 * Get the position.
		 * @return position
		 */
		virtual auto Position() -> Pair<int> { return m_Position; }

	private:
		Pair<int> m_Size{ 30, 30 }, m_Position{ 0, 0 };
	};

	/**
	 * Div is used for the layout of the Effects, allows for recursive layout structures
	 * with easy alignment of parameters and other EffectObjects
	 */
	struct Div;
	struct Div : public Object
	{
		/**
		 * When CellSize is set to AUTO (-1), the layout interpretter will equally divide
		 * available space between all Divs marked AUTO within a Div.
		 */
		static inline int AUTO = -1;

		/**
		 * Allignment of objects inside of a Div.
		 */
		enum class Alignment
		{
			Left, Right, Top, Bottom, Center, // For an Object only
			Vertical, Horizontal			  // For Divs only.
		};

		/**
		 * Type of Div. Either containing a single Object or more Divs.
		 */
		enum class Type
		{
			Divs, Object
		};

		/**
		 * Constructor.
		 * @param t alignment
		 */
		Div(Alignment t = Alignment::Horizontal)
			: m_Align(t)
		{}

		/**
		 * Get the type of this Div, either Divs or Object.
		 */
		auto DivType() -> Type { return m_Type; }
		
		/**
		 * Depending on the Div type: Set the alignment of the Divs contained in this Div or the alignment of the Object in this Div.
		 * @param a alignment
		 */
		void Align(Alignment a) { m_Align = a; }

		/**
		 * Get the current alignment of this Div.
		 */
		auto Align() -> Alignment { return m_Align; }

		/**
		 * Get the object if this Div is of type Object.
		 * @return object
		 */
		auto Object() -> Object& { return *m_Object; }

		/**
		 * Set the object if this Div is of type Object.
		 * @param o object
		 */
		void Object(Effects::Object* o)
		{
			if (m_Align == Alignment::Vertical || m_Align == Alignment::Horizontal) 
				m_Align = Alignment::Center;
			m_Type = Type::Object; m_Object = o;
		}

		/**
		 * Set the object if this Div is of type Object.
		 * @param o object
		 */
		void Object(Effects::Object& o) 
		{
			if (m_Align == Alignment::Vertical || m_Align == Alignment::Horizontal)
				m_Align = Alignment::Center;
			m_Type = Type::Object; m_Object = &o; 
		}

		/**
		 * Set padding for this Div.
		 * @param s padding
		 */
		void Padding(int s) { m_Padding = s; }

		/**
		 * Get padding for this Div.
		 * @return padding
		 */
		int  Padding() { return m_Padding; }

		/**
		 * When Dividers is set to true, the interpretter will put dividers between the Divs
		 * contained in this Div.
		 * @param s dividers
		 */
		void Dividers(bool s) { m_Dividers = s; }

		/**
		 * Returns true when dividers should be displayed between the Divs inside this Div.
		 * @returns true if has dividers
		 */
		bool Dividers() { return m_Dividers; }

		/**
		 * Set the size of this Div in pixels.
		 * @param s size
		 */
		void DivSize(int s) { m_CellSize = s; }

		/**
		 * Get the size of this Div in pixels.
		 * @return size
		 */
		int  DivSize() { return m_CellSize; }

		/**
		 * Returns all the Divs inside this Div.
		 * @return all the Divs in this Div
		 */
		auto Divs() ->std::vector<std::unique_ptr<Div>>& { return m_Divs; }

		/**
		 * Set the amount of Divs in this Div.
		 * @param i amount of Divs
		 */
		void Divs(int i)
		{
			m_Cells = i;
			m_Type = Type::Divs;
			while (m_Divs.size() < i)
				m_Divs.emplace_back(std::make_unique<Div>());
		}

		/**
		 * Get the Div at the index.
		 * @param index index
		 */
		Div& operator[](int index) { return *m_Divs[index]; }

		/**
		 * Set the object if this Div is of type Object.
		 * @param o object
		 */
		void operator=(Effects::Object& o) { Object(o); }

		/**
		 * Set the object if this Div is of type Object.
		 * @param o object
		 */
		void operator=(Effects::Object* o) { Object(o); }

	private:
		std::vector<std::unique_ptr<Div>> m_Divs;
		Effects::Object* m_Object;
		Type m_Type = Type::Divs;
		Alignment m_Align = Alignment::Center;
		int m_CellSize = AUTO;
		int m_Cells = 0;
		int m_Padding = 0;
		bool m_Dividers = false;
	};

	/**
	 * Parameter base class, anything that has a range and a value, slider/knob etc.
	 */
	class Parameter : public Object
	{
	public:

		/**
		 * Constructor.
		 * @param name name
		 * @param t type
		 */
		Parameter(const std::string& name = "", ParameterType t = ParameterType::Knob)
			: m_Name(name), m_Type(t)
		{}

		/**
		 * Set the name of this Parameter.
		 * @param n name
		 */
		virtual void Name(const std::string& n) { m_Name = n; }

		/**
		 * Get the name of this Parameter.
		 * @return name
		 */
		virtual auto Name() -> std::string& { return m_Name; }

		/**
		 * Set the range of this Parameter.
		 * @param r range
		 */
		virtual void Range(const Pair<double>& r) { m_Range = r; ConstrainValue(); }

		/**
		 * Get the range of this Parameter.
		 * @return range
		 */
		virtual auto Range() -> Pair<double> const { return m_Range; }

		/**
		 * Set the value of this Parameter.
		 * @param v value
		 */
		virtual void Value(double v) { m_Value = Normalize(v); ConstrainValue(); }

		/**
		 * Get the value of this Parameter.
		 * @return value
		 */
		virtual double Value() const { return Convert(m_Value); }

		/**
		 * Set the normalized value of this Parameter.
		 * @param v normalized value
		 */
		virtual void NormalizedValue(double v) { m_Value = v; ConstrainValue(); }

		/**
		 * Get the normalized value of this Parameter.
		 * @return normalized value
		 */
		virtual double NormalizedValue() const { return m_Value; }

		/**
		 * Set the reset-value of this Parameter.
		 * @param v reset-value
		 */
		virtual void ResetValue(double v) { m_ResetValue = v; }

		/**
		 * Reset the value to the reset-value of this Parameter.
		 */
		virtual void ResetValue() { m_Value = Normalize(m_ResetValue); }

		/**
		 * Set a Multiplier for the speed at which the parameter value 
		 * changes per pixel dragged with the mouse.
		 * @param v multiplier
		 */
		virtual void Multiplier(double v) { m_Mult = v; }

		/**
		 * Get the Multiplier for the speed at which the parameter value
		 * changes per pixel dragged with the mouse.
		 * @return multiplier
		 */
		virtual double Multiplier() { return m_Mult; }

		/**
		 * Set a power to this parameter's value range, useful when a range 
		 * is non-linear.
		 * @param v power
		 */
		virtual void   Power(double v) { m_Power = v; }

		/**
		 * Get the power to this parameter's value range.
		 * @return power
		 */
		virtual double Power() { return m_Power; }

		/**
		 * Is this parameter displayed/dragged vertically?
		 */
		virtual bool Vertical() { return m_Vertical; }

		/**
		 * Is this parameter displayed/dragged vertically?
		 * @param v vertical
		 */
		virtual void Vertical(bool v) { m_Vertical = v; }

		/**
		 * Set a Unit to display behind the value of this Parameter.
		 * the 'tenp' determines the power of 10 at which this unit is displayed.
		 * For example, ("kHz", 3) and ("Hz", 0) will display Hz up to 1000, and after that kHz.
		 * @param str unit
		 * @param tenp power of 10
		 */
		virtual void Unit(const std::string& str, int tenp = 0) { m_Units.emplace(tenp, str); }

		/**
		 * Get a map of all units defined for this Parameter.
		 * @return all units
		 */
		virtual auto Units() -> std::unordered_map<int, std::string>& { return m_Units; }

		/**
		 * Set the amount of decimals that should be displayed for this Parameter.
		 * @param d decimals
		 */
		virtual void Decimals(int d) { m_Decimals = d; }

		/**
		 * Get the amount of decimals that will be displayed for this Parameter.
		 * @return decimals
		 */
		virtual int Decimals() { return m_Decimals; }

		/**
		 * Returns true when the value should be displayed.
		 * @return true when value should be displayed
		 */
		virtual bool DisplayValue() { return m_DisplayValue; }

		/**
		 * Enable/Disable the displaying of the value.
		 * @param v display value
		 */
		virtual void DisplayValue(bool v) { m_DisplayValue = v; }

		/**
		 * Returns true when the name should be displayed.
		 * @return true when value should be displayed
		 */
		virtual bool DisplayName() { return m_DisplayName; }

		/**
		 * Enable/Disable the displaying of the name.
		 * @param v display name
		 */
		virtual void DisplayName(bool v) { m_DisplayName = v; }

		/**
		 * Returns true when this Parameter is disabled.
		 * @return true when disabled
		 */
		virtual bool Disabled() { return m_Disabled; }

		/**
		 * Disable this Parameter.
		 */
		virtual void Disable() { m_Disabled = true; }

		/**
		 * Enable this Parameter.
		 */
		virtual void Enable() { m_Disabled = false; }

		/**
		 * Returns the type of this Parameter.
		 * @return type
		 */
		virtual auto Type() -> ParameterType { return m_Type; }

	protected:
		int m_Decimals = 1;

		Pair<double> m_Range{ 0, 100 };

		double m_Value = 0,
			m_Power = 1,
			m_ResetValue = 0,
			m_Mult = 1;

		bool m_Vertical = true,
			m_DisplayValue = true,
			m_DisplayName = true,
			m_Disabled = false;

		std::string m_Name;
		std::unordered_map<int, std::string> m_Units;

		const ParameterType m_Type;

		void ConstrainValue() { m_Value = constrain(m_Value, 0, 1); }
		double Convert(double v) const { return std::powf(v, m_Power) * (m_Range.end - m_Range.start) + m_Range.start; }
		double Normalize(double v) const { return std::powf((v - m_Range.start) / (m_Range.end - m_Range.start), 1.0 / m_Power); }
	};

	/**
	 * A volume slider, with a display of a level meter with variable amount of channels.
	 */
	class VolumeSlider : public Parameter
	{
	public:

		/**
		 * Set the amount of channels.
		 * @param n channels.
		 */
		void Channels(int n)
		{
			m_Channels = n;
			while (m_Values.size() < n)
				m_Values.push_back(0);
			while (m_Reduces.size() < n)
				m_Reduces.push_back(0);
		}

		/**
		 * Get the amount of channels for this VolumeSlider.
		 * @return amount of channels
		 */
		int  Channels() { return m_Channels; }

		/**
		 * Get the list of levels of all channels in this VolumeSlider.
		 * These will be displayed behind the adjusted levels.
		 * @return levels
		 */
		auto Values() -> std::vector<float> { return m_Values; }

		/**
		 * Get the list of adjusted levels of all channels in this VolumeSlider.
		 * These will be displayed in front of the levels.
		 * @return adjusted levels
		 */
		auto Reduces() -> std::vector<float> { return m_Reduces; }

		/**
		 * Set a level for channel i.
		 * @param i channel
		 * @param v level
		 */
		void SetValue(int i, float v) { m_Values[i] = v; }

		/**
		 * Set an adjusted level for channel i.
		 * @param i channel
		 * @param v adjusted level
		 */
		void SetReduce(int i, float v) { m_Reduces[i] = v; }

	private:
		int m_Channels;
		std::vector<float> m_Values;
		std::vector<float> m_Reduces;
	};

	/**
	 * DropDown button with a group of radio buttons inside.
	 */
	class DropDown : public Object
	{
	public:

		/**
		 * An single option for the group of radio buttons.
		 */
		class Option
		{
		public:

			/**
			 * Constructor.
			 * @param name name
			 * @param i value
			 */
			Option(const std::string& name, int i)
				: id(i), name(name)
			{}

			int id;
			std::string name;
		};

		/**
		 * Add an option to the dropdown
		 * @param name name
		 * @param i value of this option
		 */
		void AddOption(const std::string& name, int i)
		{
			if (m_Selected == -1)
				m_Selected = (int)i;
			m_Options.emplace_back(name, (int)i);
		}

		/**
		 * Select the option with value i.
		 * @param i value of option to select
		 */
		void Select(int i)
		{
			m_Selected = (int)i;
		}

		/**
		 * Get the selected option.
		 * @return value of selected option
		 */
		int Selected() { return m_Selected; }

		/**
		 * Returns all options.
		 * @return all options
		 */
		auto Options() -> std::vector<Option>& { return m_Options; }

	private:
		int m_Selected = -1;
		std::vector<Option> m_Options;
	};

	/**
	 * Simple toggleable button.
	 */
	class ToggleButton : public Object
	{
	public:

		/**
		 * Constructor.
		 * @param n name
		 */
		ToggleButton(const std::string& n)
			: m_Name(n)
		{}

		/**
		 * Set the state of this button.
		 * @param s state
		 */
		void State(bool s) { state = s; }

		/**
		 * Get the state of this button.
		 * @return state
		 */
		bool State() { return state; }

		/**
		 * Get the name of this button.
		 * @return name
		 */
		auto Name() -> std::string& { return m_Name; }

		/**
		 * The state of this toggle button.
		 */
		bool state = false;

	private:
		std::string m_Name;
	};

	/**
	 * Simple radio button.
	 */
	class RadioButton : public Object
	{
	public:

		/**
		 * Constructor
		 * @param n name
		 * @param id id of the group of buttons this is part of
		 * @param callback callback
		 */
		RadioButton(const std::string& n, int id, std::function<void()> callback = [] {})
			: m_Name(n), m_Id(id), Callback(callback)
		{}

		/**
		 * Select this button in the group of buttons with the same id. 
		 * Will call the callback!
		 * @param s select
		 */
		void Selected(bool s) { selected = s; if (s) Callback(); }

		/**
		 * Returns true when this button is selected.
		 * @return true when selected
		 */
		bool Selected() { return selected; }

		/**
		 * Returns the id of the group of buttons this button is part of.
		 * @return id
		 */
		int  Id() { return m_Id; }

		/**
		 * Returns name
		 * @return name
		 */
		auto Name() -> std::string& { return m_Name; }

		/**
		 * The callback.
		 */
		std::function<void()> Callback;

		/**
		 * True when this button is selected. Use Selected(bool s) when changing the state!
		 * This is simply exposed so the Gui component can directly link with this boolean.
		 */
		bool selected = false;

	private:
		int m_Id;
		std::string m_Name;
	};

	/**
	 * Simple X-Y controller.
	 */
	class XYController : public Object
	{
	public:

		/**
		 * Constructor
		 * @param p1 parameter to control the x-axis
		 * @param p2 parameter to control the y-axis
		 */
		XYController(Parameter& p1, Parameter& p2)
			: p1(p1), p2(p2)
		{}

		/**
		 * Get the parameter for the x-axis.
		 * @return param1
		 */
		Parameter& Param1() { return p1; }

		/**
		 * Get the parameter for the y-axis.
		 * @return param2
		 */
		Parameter& Param2() { return p2; }

	private:
		Parameter& p1;
		Parameter& p2;
	};

	/**
	 * Complicated custom slider for the Dynamics Effect.
	 */
	class DynamicsSlider : public Object
	{
	public:
		void   ExpanderThreshhold(double v) { expanderThreshhold = v; }
		double ExpanderThreshhold() { return expanderThreshhold; }
		void   CompressorThreshhold(double v) { compressThreshhold = v; }
		double CompressorThreshhold() { return compressThreshhold; }
		void   ExpanderRatio(double r) { expanderRatio = r; }
		double ExpanderRatio() { return expanderRatio; }
		void   CompressorRatio(double r) { compressRatio = r; }
		double CompressorRatio() { return compressRatio; }
		void   AttackTime(double a) { attms = a; }
		double AttackTime() { return attms; }
		void   ReleaseTime(double a) { relms = a; }
		double ReleaseTime() { return relms; }
		void   PreGain(double a) { pregain = a; }
		double PreGain() { return pregain; }
		void   PostGain(double a) { postgain = a; }
		double PostGain() { return postgain; }
		void   Mix(double a) { mix = a; }
		double Mix() { return mix; }
		double Channels() { return channels; }
		auto   Levels() -> std::vector<float>& { return levels; }
		void   Level(int i, float v) { levels[i] = v; }

		void Channels(int i)
		{
			channels = i;
			while (levels.size() < i)
				levels.push_back(0);
		}

	private:
		double expanderThreshhold = -50;
		double compressThreshhold = -10;
		double expanderRatio = 8.0 / 1.0;
		double compressRatio = 1.0 / 8.0;

		double attms = 1;
		double relms = 100;

		double pregain = 0;
		double postgain = 0;
		double mix = 0;

		int channels = 0;
		std::vector<float> levels;
	};

	/**
	 * Base for any Effect.
	 */
	class EffectBase
	{
	public:

		/**
		 * Constructor
		 * @param name name
		 */
		EffectBase(const std::string& name) :
			m_Name(name)
		{};

		/**
		 * This is called each frame.
		 */
		virtual void Update() = 0;

		/**
		 * Generate the next sample.
		 * @param in sample
		 * @param c channel
		 * @return next sample
		 */
		virtual float NextSample(float in, int c) = 0;

		/**
		 * This is called whenever the amount of channels changes. 
		 * Use this to update any buffers you have to make sure you have enough space 
		 * for 'c' channels
		 * @param c channels
		 */
		virtual void Channels(int c) {}

		/**
		 * This operator is used to save the settings of this Effect.
		 */
		virtual operator nlohmann::json() = 0;

		/**
		 * This operator is used to load the settings of this Effect.
		 */
		virtual void operator=(const nlohmann::json& json) = 0;

		/**
		 * Set the samplerate.
		 * @param s samplerate
		 */
		void SampleRate(double s) { m_SampleRate = s; }

		/**
		 * Get the samplerate.
		 * @return samplerate
		 */
		double SampleRate() { return m_SampleRate; }

		/**
		 * Get the name of this Effect.
		 * @return name
		 */
		auto Name() -> const std::string& { return m_Name; }

		/**
		 * Set the height of this Effect.
		 * @param h height
		 */
		void Height(int h) { m_Height = h; }

		/**
		 * Get the height of this Effect.
		 * @return height
		 */
		int Height() { return m_Height; }

		/**
		 * Get the width of this Effect.
		 * @return width
		 */
		int Width() { return 300; }

		/**
		 * Get the layout Div of this Effect.
		 * @return div
		 */
		auto Div() -> Effects::Div& { return m_Div; }

		/**
		 * Emplace a Parameter.
		 * @param name name
		 * @param type type
		 */
		Effects::Parameter& Parameter(const std::string& name, ParameterType type)
		{
			return dynamic_cast<Effects::Parameter&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::Parameter>(name, type)));
		}

		/**
		 * Emplace a DropDown.
		 */
		Effects::DropDown& DropDown()
		{
			return dynamic_cast<Effects::DropDown&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::DropDown>()));
		}

		/**
		 * Emplace a Toggle Button.
		 * @param name name
		 */
		Effects::ToggleButton& Toggle(const std::string& name)
		{
			return dynamic_cast<Effects::ToggleButton&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::ToggleButton>(name)));
		}

		/**
		 * Emplace a VolumeSlider.
		 */
		Effects::VolumeSlider& VolumeSlider()
		{
			return dynamic_cast<Effects::VolumeSlider&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::VolumeSlider>()));
		}

		/**
		 * Emplace a DynamicsSlider.
		 */
		Effects::DynamicsSlider& DynamicsSlider()
		{
			return dynamic_cast<Effects::DynamicsSlider&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::DynamicsSlider>()));
		}

		/**
		 * Emplace a RadioButton.
		 * @param name name
		 * @param id group id
		 * @param callback callback
		 */
		Effects::RadioButton& RadioButton(const std::string& name, int id, std::function<void()> callback = [] {})
		{
			return dynamic_cast<Effects::RadioButton&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::RadioButton>(name, id, callback)));
		}

		/**
		 * Emplace a XYController.
		 * @param p1 x-axis parameter
		 * @param p2 y-axis parameter
		 */
		Effects::XYController& XYController(Effects::Parameter& p1, Effects::Parameter& p2)
		{
			return dynamic_cast<Effects::XYController&>(*m_EffectObjects.emplace_back(std::make_unique<Effects::XYController>(p1, p2)));
		}

		/**
		 * Get all objects in this Effect.
		 * @return objects
		 */
		std::vector<std::unique_ptr<Effects::Object>>& Objects()
		{
			return m_EffectObjects;
		}

	protected:
		Effects::Div m_Div;
		std::vector<std::unique_ptr<Effects::Object>> m_EffectObjects;
		const std::string m_Name = "";
		double m_SampleRate = 48000;
		int m_Height = 145;
	};
}
