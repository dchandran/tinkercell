/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.sbml.libsbml;

/** 
 * Read/write/manipulate RDF annotations stored in SBML
 * annotation elements.
 * <p>
 * <em style='color: #555'>
This class of objects is defined by libSBML only and has no direct
equivalent in terms of SBML components.  This class is not prescribed by
the SBML specifications, although it is used to implement features
defined in SBML.
</em>

 * <p>
 * RDFAnnotationParser is a libSBML construct used as part of the libSBML
 * support for annotations conforming to the guidelines specified by MIRIAM
 * ('Minimum Information Requested in the Annotation of biochemical
 * Models', <i>Nature Biotechnology</i>, vol. 23, no. 12, Dec. 2005).  Section 6
 * of the SBML Level&nbsp;2 Version&nbsp;4 specification defines a recommended way
 * of encoding MIRIAM information as RDF annotations in SBML.  The general
 * scheme is as follows.  A set of RDF-based annotations attached to a
 * given SBML <code>&lt;annotation&gt;</code> element are read by
 * RDFAnnotationParser and converted into a list of {@link CVTerm} objects.  There
 * are different versions of the main method,
 * {@link RDFAnnotationParser#parseRDFAnnotation(XMLNode annotation, List CVTerms)}
 * and {@link RDFAnnotationParser#parseRDFAnnotation(XMLNode annotation)}, 
 * used depending on whether the annotation in question concerns the MIRIAM
 * model history or other MIRIAM resource annotations.  A special object
 * class, {@link ModelHistory}, is used to make it easier to manipulate model
 * history annotations.
 * <p>
 * All of the methods on RDFAnnotationParser are static; the class exists
 * only to encapsulate the annotation and {@link CVTerm} parsing and manipulation
 * functionality.
 */

public class RDFAnnotationParser {
   private long swigCPtr;
   protected boolean swigCMemOwn;

   protected RDFAnnotationParser(long cPtr, boolean cMemoryOwn)
   {
     swigCMemOwn = cMemoryOwn;
     swigCPtr    = cPtr;
   }

   protected static long getCPtr(RDFAnnotationParser obj)
   {
     return (obj == null) ? 0 : obj.swigCPtr;
   }

   protected static long getCPtrAndDisown (RDFAnnotationParser obj)
   {
     long ptr = 0;

     if (obj != null)
     {
       ptr             = obj.swigCPtr;
       obj.swigCMemOwn = false;
     }

     return ptr;
   }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libsbmlJNI.delete_RDFAnnotationParser(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  
  /**
   * Parses an annotation into a {@link ModelHistory} class instance.
   * <p>
   * This is used to take an annotation that has been read into an SBML
   * model, identify the RDF elements representing model history
   * information, and create a list of corresponding CVTerms.
   * <p>
   * @param annotation {@link XMLNode} containing the annotation.
   * <p>
   * @return a pointer to the {@link ModelHistory} created.
   */
 public static ModelHistory parseRDFAnnotation(XMLNode annotation) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_parseRDFAnnotation__SWIG_0(XMLNode.getCPtr(annotation), annotation);
    return (cPtr == 0) ? null : new ModelHistory(cPtr, false);
  }

  
  /**
   * Creates a blank annotation and returns the {@link XMLNode} corresonding to it.
   * <p>
   * The annotation created by this method is a completely empty SBML
   * <code>&lt;annotation&gt;</code> element.  One use for this is to
   * then call createRDFAnnotation() to construct RDF content for this
   * empty annotation.
   * <p>
   * @return a pointer to an {@link XMLNode} for the annotation
   * <p>
   * @see #createRDFAnnotation()
   */
 public static XMLNode createAnnotation() {
    long cPtr = libsbmlJNI.RDFAnnotationParser_createAnnotation();
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Creates blank RDF annotation content organized in the form defined in
   * Section 6 of the SBML Level 2 Version 4 specification .
   * <p>
   * The annotation created by this method has namespace declarations for
   * all the relevant XML namespaces used in RDF annotations and also has
   * an empty RDF element.  Note that this is not the containing
   * <code>&lt;annotation&gt;</code> element; the method createAnnotation()
   * is available for that purpose.
   * <p>
   * @return a pointer to an {@link XMLNode}
   */
 public static XMLNode createRDFAnnotation() {
    long cPtr = libsbmlJNI.RDFAnnotationParser_createRDFAnnotation();
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Deletes any RDF annotation found in the given {@link XMLNode} tree and returns
   * any remaining annotation content.
   * <p>
   * The name of the given {@link XMLNode} must be 'annotation', or else this
   * method returns NULL.
   * <p>
   * @param annotation the annotation tree within which the RDF annotation
   * is to be found and deleted
   * <p>
   * @return the {@link XMLNode} structure with any RDF annotations deleted
   */
 public static XMLNode deleteRDFAnnotation(XMLNode annotation) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_deleteRDFAnnotation(XMLNode.getCPtr(annotation), annotation);
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Takes an SBML object and creates an {@link XMLNode} corresponding to an
   * RDF 'Description' element.
   * <p>
   * This method is a handy way of creating RDF description objects linked
   * by the appropriate 'metaid' field, for insertion into RDF annotations
   * in a model.  (Note that this method does not create a complete
   * annotation; it only creates a description element.  For creating empty
   * RDF annotations that can serve as containers for RDF descriptions, see
   * createRDFAnnotation().
   * <p>
   * @param object the object to be annotated
   * <p>
   * @return a new {@link XMLNode} containing the 'rdf:about' structure for an
   * RDF 'Description' element.
   * <p>
   * @see #createRDFAnnotation()
   */
 public static XMLNode createRDFDescription(SBase object) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_createRDFDescription(SBase.getCPtr(object), object);
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Takes a list of CVTerms and creates a the RDF 'Description' element.
   * <p>
   * This essentially takes the given SBML object, reads out the CVTerms
   * attached to it, calls createRDFDescriptiom() to create an RDF
   * 'Description' element to hold the terms and adds each term with
   * appropriate qualifiers.
   * <p>
   * @param object the SBML object to start from
   * <p>
   * @return the {@link XMLNode} tree corresponding to the Description element of
   * an RDF annotation.
   */
 public static XMLNode createCVTerms(SBase object) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_createCVTerms(SBase.getCPtr(object), object);
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Takes a list of CVTerms and creates a complete SBML annotation
   * around it.
   * <p>
   * This essentially takes the given SBML object, calls createCVTerms
   * to read out the CVTerms
   * attached to it, calls createRDFAnnotation() to create an RDF
   * annotation to hold the terms, and finally calls createAnnotation() to
   * wrap the result as an SBML <code>&lt;annotation&gt;</code> element.
   * <p>
   * @param object the SBML object to start from
   * <p>
   * @return the {@link XMLNode} tree corresponding to the annotation.
   */
 public static XMLNode parseCVTerms(SBase object) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_parseCVTerms(SBase.getCPtr(object), object);
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  
  /**
   * Takes an SBML {@link Model} object, reads off the model history information
   * stored in it, and creates a complete SBML annotation to store that
   * history.
   * <p>
   * @param object a {@link Model}
   * <p>
   * @return the {@link XMLNode} corresponding to an annotation containing 
   * MIRIAM-compliant model history information in RDF format
   */
 public static XMLNode parseModelHistory(Model object) {
    long cPtr = libsbmlJNI.RDFAnnotationParser_parseModelHistory(Model.getCPtr(object), object);
    return (cPtr == 0) ? null : new XMLNode(cPtr, true);
  }

  public static void parseRDFAnnotation(XMLNode annotation, CVTermList CVTerms) {
    libsbmlJNI.RDFAnnotationParser_parseRDFAnnotation__SWIG_1(XMLNode.getCPtr(annotation), annotation, CVTermList.getCPtr(CVTerms), CVTerms);
  }

  public RDFAnnotationParser() {
    this(libsbmlJNI.new_RDFAnnotationParser(), true);
  }

}