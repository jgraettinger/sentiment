
import mako.exceptions

class TemplateResolver(object):
    """
    This class instruments a poor-mans 'polymorphism' concept
    when working with mako templates.

    If related templates (methods) are layed out within
    a resource (class) subdirectory:

        /html
            /general_resource
                /index.mako
                /new.mako
                /edit.mako
                /show.mako
                /create.mako
                /update.mako
                /delete.mako

    Then other resource types can 'subclass', by re-implementing
    a subset of templates of particular interest, and defering
    to the 'base' resource template otherwise.

        /html
            /specific_resource
                /index.mako
                /show.mako

    Moreover, an html 'show' action may require specialized javascript,
    but a json 'show' action might just want to use the base's
    simplejson functionality.

    Clients of TemplateResolver provide a mako TemplateLookup, and
    a list of paths to directories containing mako templates. This
    list of paths is exactly akin to a method resolution order.

    At resolution time, an appropriate template is looked for
    in resolution order, and returned.
    """

    def __init__(self, template_lookup, resolution_order):
        self.template_lookup = template_lookup
        self.resolution_order = resolution_order
        return

    def resolve(self, request, name):
        req_format = request.route_args['format']

        for template_path in self.resolution_order:
            full_path = '/%s/%s/%s' % (req_format, template_path, name)
            if self.template_lookup.has_template(full_path):
                return self.template_lookup.get_template(full_path)

        raise mako.exceptions.TopLevelLookupException(
            "Can't locate a template for %r, with "\
            "format %r and resolution order %r" % (
                name, req_format, self.resolution_order))

