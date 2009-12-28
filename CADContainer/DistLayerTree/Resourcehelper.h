#ifndef __RESOURCE_HELPER__H__
#define __RESOURCE_HELPER__H__

class CTemporaryResourceOverride
{
public:
    CTemporaryResourceOverride(HINSTANCE hInstNew);
    CTemporaryResourceOverride(); // default construction

    virtual ~CTemporaryResourceOverride();

    static void SetDefaultResource(HINSTANCE hInstNew);

private:
    void   CommonConstruction(HINSTANCE);
    static HINSTANCE m_hInstanceDefault;

    HINSTANCE m_hInstanceOld;
};

#endif // __RESOURCE_HELPER__H__

HINSTANCE CTemporaryResourceOverride::m_hInstanceDefault = NULL;

void CTemporaryResourceOverride::SetDefaultResource(HINSTANCE hInstDefault)
// For using the default constructor, you've got to use this guy once.
// Typically, you do this once in your DLLMain function....
{
    ASSERT(hInstDefault != NULL);

    m_hInstanceDefault = hInstDefault;
}

void CTemporaryResourceOverride::CommonConstruction(HINSTANCE hInst)
// Handles the various flavors of construction with a common handler
{
    // Here, we scurry away the old resource handle.
    m_hInstanceOld = AfxGetResourceHandle();
    AfxSetResourceHandle(hInst);
}

CTemporaryResourceOverride::CTemporaryResourceOverride(HINSTANCE hInstNew)
// If you have a specific instance in mind, use this variant.
{
    ASSERT(hInstNew != NULL);

    CommonConstruction(hInstNew);
}

CTemporaryResourceOverride::CTemporaryResourceOverride()
// If you're always using the same instance, set the default, and use this variant.
{
    ASSERT(m_hInstanceDefault != NULL);

    CommonConstruction(m_hInstanceDefault);
}

CTemporaryResourceOverride::~CTemporaryResourceOverride()
// And here's where we clean up.  Nice and simple.
{
    AfxSetResourceHandle(m_hInstanceOld);
}
