
# all freq are in MHz unless otherwise stated

################################################################
# configure inputs and outputs here
################################################################

desired_sysclock_freq_min  = 160
desired_sysclock_freq_max  = 180

# this is assumed to be true always.
# todo: use this flag to alter validity settings
pll48_clock_needed         = True

#output for the pll prescalar (m) requires 1-2MHz. if this is set
# to true, then we will make sure only 2MHz output configs are used.
# this will minimaize pll jitter
enforce_minimal_pll_jitter  = True

#internal, external, ... doesnt matter. only the source freq is
# relevant here
clock_source_freq = 8.0

################################################################
# hardware defined settings
################################################################

VCO_OUTPUT_FREQ_MIN        = 100
VCO_OUTPUT_FREQ_MAX        = 432

PLL_P_FREQ_MAX             = 180

PLL_M_OUTPUT_FREQ_MIN      = 1
PLL_M_OUTPUT_FREQ_MAX      = 2

PLL_M_VALUES = range(2,64)
PLL_N_VALUES = range(50,433)
PLL_Q_VALUES = range(2,16)
PLL_P_VALUES = [2,4,6,8]


def get_valid_pll_m_range(incoming_freq):

    accepted_m_values = []

    for m in PLL_M_VALUES:

        # divide incoming freq by m prescaler to get outpt freq
        output_freq = incoming_freq / m

        # check that output freq is valid
        if (enforce_minimal_pll_jitter):
            # lowest pll jitter is max pll m output freq
            if (output_freq == PLL_M_OUTPUT_FREQ_MAX):
                accepted_m_values.append(m)
        else:
            # is it within hardware acceptable range
            if ((output_freq >= PLL_M_OUTPUT_FREQ_MIN) and (output_freq <= PLL_M_OUTPUT_FREQ_MAX)):
                accepted_m_values.append(m)

    # return list of acceptable m prescalar values

    return accepted_m_values

def get_valid_pll_n_range(incoming_freq):

    accepted_n_values = []

    for n in PLL_N_VALUES:
        output_freq = incoming_freq * n

        if ((output_freq >= VCO_OUTPUT_FREQ_MIN) and (output_freq <= VCO_OUTPUT_FREQ_MAX)):
            accepted_n_values.append(n)

    return accepted_n_values


def get_valid_pll_p_range( incoming_freq ):

    accepted_p_values = []


    for p in PLL_P_VALUES:
        output_freq = incoming_freq / p

        if ((output_freq >= desired_sysclock_freq_min) and (output_freq <= desired_sysclock_freq_max)):
            accepted_p_values.append(p)


    return accepted_p_values


def get_valid_pll_q_range( incoming_freq ):

    accepted_q_values = []

    for q in PLL_Q_VALUES:
        output_freq = incoming_freq / q

        #print("48mhz bus freq = " + str(output_freq))

        if (output_freq == 48):
            accepted_q_values.append(q)

    return accepted_q_values


def main():


    # find valid m prescaler values based on our clock source
    m_values = get_valid_pll_m_range( clock_source_freq )

    if (len(m_values) == 0):
        print("no possible m values");
        os._exit(2)

    for m in m_values:

        vco_input_freq = clock_source_freq / m

        n_values = get_valid_pll_n_range( vco_input_freq )

        # we didnt get any n values that work
        if (len(n_values) == 0):
            continue;

        for n in n_values:

            vco_output_freq = vco_input_freq * n

            p_values = get_valid_pll_p_range( vco_output_freq )

            if (len(p_values) == 0):
                continue

            #print("found " + str(len(p_values)) + " valid p values. " + str(p_values))
            #print("sysclock = " + str(vco_output_freq / p_values[0]))

            q_values = get_valid_pll_q_range( vco_output_freq )

            if (len(q_values) == 0):
                continue

            #print("found valid q values")

            print("##############################################################")
            print("Found Good Config. m=" + str(m) + ". n=" + str(n) + ". p_values:")
            print(p_values)
            print("q values")
            print(q_values)
            print("sysclock = " + str(vco_output_freq / p_values[0]) )

    print("done")

main()

