package unique;

import java.util.HashMap;
import java.util.Vector;

public class UniqueIdCreator {
    private HashMap<String, Vector<UniqueId>> m_used;

    public UniqueIdCreator()
    {
        m_used = new HashMap<>();
    }

    public void setUsed(UniqueId id)
    {
        if (m_used.containsKey(id.prefix())) {
            if (!m_used.get(id.prefix()).contains(id)) {
                m_used.get(id.prefix()).add(id);
            }
        }
        else {
            Vector<UniqueId> tmpVec = new Vector<>();

            tmpVec.add(id);
            m_used.put(id.prefix(), tmpVec);
        }
    }

    public UniqueId genNew(String prefix)
    {
        if (!m_used.containsKey(prefix)) {
            UniqueId tmpId = new UniqueId(prefix, 0);
            Vector<UniqueId> tmpVec = new Vector<>();

            tmpVec.add(tmpId);
            m_used.put(prefix, tmpVec);
            return tmpId;
        }

        int number = 0;

        for (UniqueId id : m_used.get(prefix)) {
            if (id.id() > number)
                number = id.id();
        }

        UniqueId tmpId = new UniqueId(prefix, number + 1);
        m_used.get(prefix).add(tmpId);
        setUsed(tmpId);

        return tmpId;
    }
}
