package android.vendor.radio;

@VintfStability
interface ITea5767Service {

    /**
     * Mutes the TEA5767 audio output.
     */
    void mute();

    /**
     * Unmutes the TEA5767 audio output.
     */
    void unmute();

    /**
     * Changes the station frequency.
     *
     * @param frequencyKHz The desired frequency in kilohertz (e.g., 89200 for 89.2 MHz).
     */
    void changeStation(int frequencyKHz);
}
