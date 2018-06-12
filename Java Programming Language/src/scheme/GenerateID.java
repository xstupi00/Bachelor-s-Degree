package scheme;

import unique.UniqueIdCreator;

/**
 * Class contains the implementation of Generator of UniquedID for the Schemes, Blocks and Ports.
 *
 * @author  Stupinský Šimon
 * @since   2017-04-28
 */
public class GenerateID {

    /**
     * Create the new UniqueID Generator.
     * @return new created generator of UniqueID
     */
    public UniqueIdCreator createGenerator() {
        UniqueIdCreator creator = new UniqueIdCreator();
        return creator;
    }
}
