namespace yake {
namespace graphics {

	/**
	*/
	class IParticleSystem
	{
	public:
		virtual ~IParticleSystem() {}

		virtual void setEnabled( bool enabled ) = 0;
	};

	enum FogMode
	{
		/// No fog. Duh.
		FM_NONE,
		/// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
		FM_EXP,
		/// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
		FM_EXP2,
		/// Fog density increases linearly between the start and end distances
		FM_LINEAR
	};

	/** Environmental effects. They can be applied to a world. They can also be applied to volumes
		within that world allowing for different environmental settings (for example, under-water,
		a cave with white fog and low ambient lighting, a desert with yellow-coloured fog and bright
		ambient lighting during the day etc.)
		\todo - design + implement the process of applying the settings to specific volumes (or the other way around)
		\todo - abstract the effects themselves (IEffect <- IEnvironmentalEffect <- IEnvFog / IEnvAmbientLighting / IRain / ...)
	*/
	class IEnvironment
	{
	public:
		virtual ~IEnvironment() {}

		// ambient lighting
		virtual void setAmbientLightColor( const Color& rColor ) = 0;
		virtual Color getAmbientLightColor() const = 0;

		// fog
		virtual void setFogMode( FogMode mode ) = 0;
		virtual void setFogColor( const Color& rColor ) = 0;
		virtual void setFogLinearStart( real distance ) = 0;
		virtual void setFogLinearEnd( real distance ) = 0;
		virtual void setFogExpDensity( real density ) = 0;
		virtual FogMode getFogMode() const = 0;
		virtual Color getFogColor() const = 0;
		virtual real getFogLinearStart() const = 0;
		virtual real getFogLinearEnd() const = 0;
		virtual real getFogExpDensity() const = 0;

		virtual void addEffect( IEffect* pEffect ) = 0;

		/** Sets the "sky" object for this environment.
			Ownership of 'pSky' is not transferred.
			\see ISky
		*/
		virtual void setSky( ISky* pSky ) = 0;
	};

	/** Represents a "sky" object, for example a sky box, a sky plane or sky dome.
		\see IEnvironment
	*/
	class ISky
	{
	public:
		virtual ~ISky() {}

		virtual void setOrientation( const Quaternion & orientation ) = 0;
		virtual Quaternion getOrientation() const = 0;

		virtual void setEnabled( bool enabled ) = 0;
	};

	/*
                     IEffect
                   /        \
                 /            \
                |              |
        IEnvironmentalEffect  IStrangeEffect
            |     |
          IRain  IFog

	*/

	class IEffect
	{
	public:
		virtual ~IEffect() {}

		virtual void setEnabled( bool enabled ) = 0;
	};

	class IEnvironmentalEffect : public IEffect
	{
	public:
	};

	class IFogEffect : public IEnvironmentalEffect
	{
	public:
	};

	class IAmbientLightingEffect : public IEnvironmentalEffect
	{
	public:
	};

}
}