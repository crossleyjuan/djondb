/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.sinco.designer.projecteditor;

import java.io.IOException;
import org.openide.filesystems.FileObject;
import org.openide.loaders.DataObjectExistsException;
import org.openide.loaders.MultiDataObject;
import org.openide.nodes.CookieSet;
import org.openide.nodes.Node;
import org.openide.util.Lookup;
import org.openide.text.DataEditorSupport;

public class JaguarProjectDataObject extends MultiDataObject {

    public JaguarProjectDataObject(FileObject pf, JaguarProjectDataLoader loader) throws DataObjectExistsException, IOException {
        super(pf, loader);
        CookieSet cookies = getCookieSet();
        //cookies.add((Node.Cookie) DataEditorSupport.create(this, getPrimaryEntry(), cookies));
        cookies.add((Node.Cookie)new JFAOpenSupport(getPrimaryEntry()));
    }

    @Override
    protected Node createNodeDelegate() {
        return new JaguarProjectDataNode(this, getLookup());
    }

    @Override
    public Lookup getLookup() {
        return getCookieSet().getLookup();
    }
}
