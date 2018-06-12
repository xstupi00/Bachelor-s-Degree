package unique;

import java.util.Objects;

public class UniqueId {
    @Override
    public int hashCode()
    {
        return Objects.hash(m_prefix, m_id);
    }

    @Override
    public boolean equals(Object o)
    {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UniqueId uniqueId = (UniqueId) o;
        return m_id == uniqueId.m_id &&
                Objects.equals(m_prefix, uniqueId.m_prefix);
    }

    public UniqueId() { }

    public UniqueId(String prefix, int id)
    {
        m_id = id;
        m_prefix = prefix;

    }

    public int id() {
        return m_id;
    }

    public String prefix() {
        return m_prefix;
    }

    private String m_prefix;
    private int m_id;
}
