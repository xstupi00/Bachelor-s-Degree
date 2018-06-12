package scheme;

import unique.UniqueId;

import java.util.HashMap;

/**
 * Class contains the implementation of Ports, which are part of Blocks
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class Port {

    /**
     * Type of the port; 0 - input port, 1 - output port
     */
    public int type = -1;

    /**
     * UniqueID of the Port
     */
    public UniqueId ID;

    /**
     * HashMap which contains the types of the Port.
     * HashMap contains the pairs key -- value, where the
     * values have Double type.
     */
    public HashMap<String, Double> dataSet = new HashMap<>();

    /**
     * UniqueID of pair port, in the case, when then port
     * is connected to other port, else is this UniquedID empty.
     */
    public UniqueId pairPort;

    /**
     * UniqueID of Block, where the Port is located.
     */
    public UniqueId blockID;

    /**
     * Update data in the HashMap of Port.
     *
     * @param key   key to the HashMap
     * @param value double value, which will be actualized
     */
    public void updateData(String key, double value) {
        this.dataSet.put(key, value);
    }

    /**
     * Return data from the HashMap of Port, according to the given key.
     *
     * @param key key to the HashMap
     * @return value from the element of HashMap with key
     */
    public double getValue(String key) {
        return this.dataSet.get(key);
    }


    /**
     * Check presence of key in the HashMap of the Port.
     * @param key for search
     * @return true if the HashMap contains the element with given key, else false
     */
    public boolean hasItem(String key) {
        if (this.dataSet.get(key) == null)
            return false;
        return true;
    }
}

